#include "neo_alloc.h"
#include "core_cm4.h"

// Declare _heap_start as a pointer to the start of the heap region
extern uint8_t _heap_start[];

/**
 * Configuration constants for the heap allocator
 */
#define HEAP_SIZE 0x10000 // 64KB total heap size
#define SPLIT_CUTOFF 8    // Minimum remaining size needed to split a chunk into two
#define DEFRAG_CUTOFF 2   // Number of free operations before automatic defragmentation

/**
 * Chunk header structure (4 bytes total)
 * The fields are arranged for optimal memory alignment:
 * - allocated: Indicates if chunk is in use (1 byte)
 * - padding: Reserved for future use (1 byte)
 * - size: Size of the chunk's data area in bytes (2 bytes)
 *
 * This arrangement ensures the size field is 2-byte aligned, which is
 * important for efficient access on ARM processors. The total header
 * remains 4 bytes to maintain alignment of the data portion.
 */
typedef struct
{
    uint8_t allocated; // 0 = free, 1 = allocated
    uint8_t padding;   // Unused, helps with alignment
    uint16_t size;     // Size of chunk data (excluding header)
} __attribute__((packed)) ChunkHeader;

// Define the heap region bounds using external symbol
static uint8_t *heap_start = &_heap_start[0];
static uint8_t *heap_end = &_heap_start[HEAP_SIZE];

// Counter for tracking free operations to trigger defragmentation
static volatile uint8_t free_calls = 0;

// Flag to indicate if the heap has been initialized
static volatile bool heap_initialized = false;

/**
 * Validates if a chunk header pointer is within the heap bounds
 * and properly aligned.
 *
 * @param header Pointer to the chunk header to validate
 * @return true if header is valid, false otherwise
 */
static bool is_valid_header(const ChunkHeader *header)
{
    return ((uint8_t *)header >= heap_start &&
            (uint8_t *)header + sizeof(ChunkHeader) <= heap_end &&
            ((uint8_t *)header - heap_start) % 4 == 0); // Check 4-byte alignment
}

/**
 * Validates if a chunk's size would exceed heap boundaries
 *
 * @param header Pointer to the chunk header to validate
 * @return true if chunk size is valid, false otherwise
 */
static bool is_valid_chunk_size(const ChunkHeader *header)
{
    return ((uint8_t *)header + sizeof(ChunkHeader) + header->size <= heap_end);
}

/**
 * Retrieves a chunk header at a given offset in the heap.
 *
 * @param offset Byte offset from heap start
 * @return Pointer to chunk header, or NULL if offset is invalid
 */
static ChunkHeader *get_header(size_t offset)
{
    if (offset >= HEAP_SIZE - sizeof(ChunkHeader))
        return NULL;

    ChunkHeader *header = (ChunkHeader *)(heap_start + offset);

    // Validate the header to ensure it's within bounds and has valid size
    if (!is_valid_header(header) || !is_valid_chunk_size(header))
        return NULL;

    return header;
}

/**
 * Coalesces adjacent free chunks to reduce memory fragmentation.
 * This function merges consecutive unallocated chunks into larger
 * free chunks, improving allocation efficiency for larger requests.
 *
 * The process:
 * 1. Scans the heap from start to end
 * 2. When finding a free chunk, checks if the next chunk is also free
 * 3. If both chunks are free, merges them by updating the first chunk's size
 * 4. Continues until no more merges are possible
 */
static void defragment(void)
{
    size_t curr_offset = 0;
    bool merged;

    do
    {
        merged = false;
        curr_offset = 0;

        while (curr_offset < HEAP_SIZE)
        {
            ChunkHeader *curr = get_header(curr_offset);
            if (!curr)
                break;

            if (!curr->allocated)
            {
                size_t next_offset = curr_offset + sizeof(ChunkHeader) + curr->size;
                ChunkHeader *next = get_header(next_offset);

                if (next && !next->allocated)
                {
                    // Merge with next chunk by absorbing its space
                    if (curr->size <= UINT16_MAX - sizeof(ChunkHeader) - next->size)
                    {
                        curr->size += sizeof(ChunkHeader) + next->size;
                        merged = true;
                    }
                }
            }

            // Always move to the next chunk to avoid infinite loop
            curr_offset += sizeof(ChunkHeader) + curr->size;
        }
    } while (merged); // Continue defragmenting until no more merges occur
}

/**
 * Safe way to enable interrupts that ensures we don't accidentally enable them
 * if they were already disabled before our critical section.
 */
static uint32_t primask_bit;

static inline void safe_disable_irq(void)
{
    primask_bit = __get_PRIMASK();
    __disable_irq();
}

static inline void safe_enable_irq(void)
{
    if (!primask_bit)
    {
        __enable_irq();
    }
}

/**
 * Initializes the heap by creating a single large free chunk.
 * This must be called before any allocation operations.
 *
 * The function:
 * 1. Safely disables interrupts to ensure thread safety
 * 2. Creates an initial free chunk spanning the entire heap
 * 3. Safely re-enables interrupts
 */
void neo_heap_init(void)
{
    safe_disable_irq();

    if (!heap_initialized)
    {
        ChunkHeader *initial = (ChunkHeader *)heap_start;
        initial->allocated = 0;
        initial->padding = 0;
        initial->size = HEAP_SIZE - sizeof(ChunkHeader);
        heap_initialized = true;
    }

    safe_enable_irq();
}

/**
 * Allocates memory from the heap with proper alignment.
 *
 * The allocation process:
 * 1. Rounds requested size up to maintain 4-byte alignment
 * 2. Searches for first free chunk large enough to hold request
 * 3. If chunk is significantly larger than needed, splits it
 * 4. Returns pointer to the allocated memory region
 *
 * @param size Requested allocation size in bytes
 * @return Pointer to allocated memory, or NULL if allocation fails
 */
void *neo_alloc(uint16_t size)
{
    if (!size || !heap_initialized)
        return NULL;

    safe_disable_irq();

    // Round size up to nearest multiple of 4 for alignment
    uint16_t aligned_size = (size + 3) & ~3;

    size_t curr_offset = 0;
    while (curr_offset < HEAP_SIZE)
    {
        ChunkHeader *curr = get_header(curr_offset);
        if (!curr)
            break;

        if (!curr->allocated && curr->size >= aligned_size)
        {
            // Check if chunk should be split to avoid wasting space
            if (curr->size >= aligned_size + sizeof(ChunkHeader) + SPLIT_CUTOFF)
            {
                size_t new_offset = curr_offset + sizeof(ChunkHeader) + aligned_size;
                ChunkHeader *new_chunk = (ChunkHeader *)(heap_start + new_offset);

                if (is_valid_header(new_chunk))
                {
                    // Initialize the new chunk from the split
                    new_chunk->allocated = 0;
                    new_chunk->padding = 0;
                    new_chunk->size = curr->size - aligned_size - sizeof(ChunkHeader);

                    // Update current chunk
                    curr->allocated = 1;
                    curr->size = aligned_size;
                }
                else
                {
                    // If new chunk header would be invalid, don't split
                    curr->allocated = 1;
                }
            }
            else
            {
                // Use entire chunk if splitting would create too small a remainder
                curr->allocated = 1;
            }

            safe_enable_irq();
            return heap_start + curr_offset + sizeof(ChunkHeader);
        }

        curr_offset += sizeof(ChunkHeader) + curr->size;
    }

    safe_enable_irq();
    return NULL;
}

/**
 * Frees previously allocated memory and potentially defragments the heap.
 *
 * The free process:
 * 1. Validates the provided pointer
 * 2. Marks the chunk as unallocated
 * 3. Increments free counter and triggers defragmentation if threshold reached
 *
 * @param ptr Pointer to memory region to free
 * @return true if successfully freed, false if invalid pointer
 */
void neo_free(void *ptr)
{
    if (!ptr || !heap_initialized)
        return;

    safe_disable_irq();

    if ((uint8_t *)ptr < heap_start + sizeof(ChunkHeader) ||
        (uint8_t *)ptr >= heap_end)
    {
        safe_enable_irq();
        return;
    }

    // Get header pointer by subtracting header size from data pointer
    ChunkHeader *header = (ChunkHeader *)((uint8_t *)ptr - sizeof(ChunkHeader));

    if (!is_valid_header(header) || !header->allocated || !is_valid_chunk_size(header))
    {
        safe_enable_irq();
        return;
    }

    header->allocated = 0;

    // Trigger defragmentation periodically to prevent excessive fragmentation
    if (++free_calls >= DEFRAG_CUTOFF)
    {
        defragment();
        free_calls = 0;
    }

    safe_enable_irq();
    return;
}

/**
 * Provides information about the current heap state.
 *
 * @param total_bytes [out] Total heap size in bytes
 * @param used_bytes [out] Currently allocated bytes
 * @param free_bytes [out] Available bytes
 * @param largest_block [out] Size of largest contiguous free block
 */
void neo_heap_info(size_t *total_bytes, size_t *used_bytes,
                   size_t *free_bytes, size_t *largest_block)
{
    size_t used = 0, free = 0, largest = 0;
    size_t curr_offset = 0;

    safe_disable_irq();

    if (!heap_initialized)
    {
        if (total_bytes)
            *total_bytes = HEAP_SIZE;
        if (used_bytes)
            *used_bytes = 0;
        if (free_bytes)
            *free_bytes = 0;
        if (largest_block)
            *largest_block = 0;
        safe_enable_irq();
        return;
    }

    while (curr_offset < HEAP_SIZE)
    {
        ChunkHeader *curr = get_header(curr_offset);
        if (!curr)
            break;

        if (curr->allocated)
        {
            used += curr->size;
        }
        else
        {
            free += curr->size;
            if (curr->size > largest)
            {
                largest = curr->size;
            }
        }

        curr_offset += sizeof(ChunkHeader) + curr->size;
    }

    if (total_bytes)
        *total_bytes = HEAP_SIZE;
    if (used_bytes)
        *used_bytes = used;
    if (free_bytes)
        *free_bytes = free;
    if (largest_block)
        *largest_block = largest;

    safe_enable_irq();
}