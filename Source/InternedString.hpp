#pragma once
#include <cstdint>
#include <string_view>
#include <array>

class InternedString final
{
public:
	static constexpr uint32_t IS_USED_MASK = 1u << 29u;
	static constexpr uint32_t MAX_STRING_SIZE = 1023u;

	struct Slot;
	struct SlotPool;
	struct StringEntryHandle;
	struct StringEntryHeader;
	struct StringEntry;
	struct HashInfo;
	struct StringEntryMemoryManager;

	struct StringEntryHandle
	{
	private:
		uint8_t empty3 : 3;
		uint16_t memoryBlockIndex13: 13;
		uint16_t memoryBlockAlignedOffset16: 16;
	public:
		inline const StringEntryHandle(uint16_t memoryBlockIndex, uint16_t memoryBlockAlignedOffset);
		inline const uint16_t GetMemoryBlockIndex() const;
		inline const uint16_t GetMemoryBlockAlignedOffset() const;
	};
	struct StringEntryHeader
	{
	private:
		uint8_t stringHashProbe6: 6;
		uint16_t size10: 10;
	public:
		inline const bool operator==(const StringEntryHeader& r) const;
		inline void SetStringHashProbe(uint8_t stringHashProbe);
		inline void SetSize(uint16_t size);
		inline const uint16_t GetSize() const;
	};
	struct StringEntry
	{
	private:
		StringEntryHeader stringEntryHeader;
		char data[MAX_STRING_SIZE];
	public:
		inline const StringEntryHeader GetStringEntryHeader() const;
		inline const char* GetData() const;
	};
	struct HashInfo
	{
	private:
		uint32_t slotIndex32;
		uint32_t slotHashProbe3_empty29;
		uint8_t slotPoolIndex8;
		StringEntryHeader stringEntryHeader;
		const char* data;
	public:
		inline HashInfo(uint64_t hash, const std::string_view& string);
		inline const uint32_t GetSlotHashProbe() const;
		inline const StringEntryHeader GetStringEntryHeader() const;
		inline const char* GetData() const;
		inline const uint32_t GetSlotIndex() const;
		inline const uint8_t GetSlotPoolIndex() const;
	};
private:
	static constexpr uint32_t MAX_SLOT_POOL_ARRAY_SIZE = 256u;
	static constexpr uint32_t MAX_MEMORY_BLOCK_ARRAY_SIZE = 8192u;
	static constexpr uint32_t SLOT_POOL_INITIALIZE_SIZE = 256u;
	static constexpr uint32_t MEMORY_BLOCK_INITIALIZE_SIZE = (1u << 16u) * 2u;
	struct Slot
	{
	private:
		uint32_t slotHashProbe3_stringEntryHandle29;
	public:
		inline const bool IsUsed() const;
		inline const bool IsTargetSlot(const HashInfo& hashInfo) const;
		inline const uint32_t GetSlotHashProbe() const;
		inline const StringEntryHandle GetStringEntryHandle() const;
	};
	struct SlotPool
	{
		friend class InternedString;
	private:
		uint32_t capcity;
		uint32_t size;
		Slot* slotArray;
	public:
		inline Slot* FindTargetOrUnusedSlot(const HashInfo& hashInfo) const;
	};
	struct StringEntryMemoryManager
	{
		friend class InternedString;
	private:
		uint16_t currentMemoryBlockIndex;
		uint16_t currentMemoryBlockCursor;
		char* memoryBlockArray[MAX_MEMORY_BLOCK_ARRAY_SIZE];
	public:
		inline const StringEntry& GetStringEntry(const StringEntryHandle& stringEntryHandle) const;
	};
	static std::array<SlotPool, MAX_SLOT_POOL_ARRAY_SIZE> _slotPoolArray;
	static StringEntryMemoryManager _stringEntryMemoryManager;
private:
	uint32_t _isNull1_StringEntryHandle29;
public:
	InternedString();
	InternedString(const std::string_view& string);
	InternedString(const InternedString& internedString);
	InternedString(InternedString&& internedString);
	~InternedString() = default;
private:
	inline static const uint32_t MakeInterned(const std::string_view& string);
	inline static const HashInfo GetHashInfo(const std::string_view& string);
public:
	static void Initialize();
};