#pragma once
#include <cstdint>
#include <string_view>
#include <array>
#include <mutex>

class InternedString final
{
private:
	static constexpr uint32_t IS_USED_MASK = 1u << 29u;
	static constexpr uint32_t STRING_ENTRY_HANDLE_BITS = 29u;
	static constexpr uint32_t STRING_ENTRY_HANDLE_MASK = (1u << 29u) - 1u;
	static constexpr uint32_t MAX_STRING_SIZE = 1023u;
	static constexpr uint32_t MAX_SLOT_POOL_ARRAY_SIZE = 256u;
	static constexpr uint32_t MAX_MEMORY_BLOCK_ARRAY_SIZE = 8192u;

	struct Slot;
	struct SlotPool;
	struct StringEntryHandle;
	struct StringEntryHeader;
	struct StringEntry;
	struct HashInfo;
	struct StringEntryMemoryManager;

	struct StringEntryHandle
	{
		static constexpr uint16_t  MEMORY_BLOCK_INDEX_BITS = 13u;
		static constexpr uint16_t  MEMORY_BLOCK_INDEX_MASK = (1u << MEMORY_BLOCK_INDEX_BITS) - 1u;
	private:
		uint16_t empty3_memoryBlockIndex13;
		uint16_t memoryBlockAlignedOffset16;
	public:
		inline StringEntryHandle();
		inline StringEntryHandle(uint32_t stringEntryHandleValue);
		inline StringEntryHandle(uint16_t memoryBlockIndex, uint16_t memoryBlockAlignedOffset);
		inline const uint16_t GetMemoryBlockIndex() const;
		inline const uint16_t GetMemoryBlockAlignedOffset() const;
		inline const uint32_t GetStringEntryHandleValue() const;
	};
	struct StringEntryHeader
	{
		static constexpr uint16_t SIZE_BITS = 10u;
		static constexpr uint16_t SIZE_MASK = (1u << SIZE_BITS) - 1u;
		static constexpr uint16_t STRING_HASH_PROBE_BITS = 6u;
		static constexpr uint16_t STRING_HASH_PROBE_MASK = ((1u << STRING_HASH_PROBE_BITS) - 1u) << 10u;
	private:
		uint16_t stringHashProbe6_size10;
	public:
		inline const bool operator==(const StringEntryHeader& r) const;
		inline void SetStringHashProbeAndSize(const uint8_t stringHashProbe, const uint16_t size);
		inline const uint16_t GetSize() const;
	};
	struct StringEntry
	{
	private:
		StringEntryHeader stringEntryHeader;
		char data[MAX_STRING_SIZE];
	public:
		inline const StringEntryHeader& GetStringEntryHeader() const;
		inline void SetStringEntryHeader(const StringEntryHeader& stringEntryHeader);
		inline const char* GetData() const;
	};
	struct HashInfo
	{
		static constexpr uint32_t SLOT_POOL_INDEX_BITS = 8u;
		static constexpr uint32_t SLOT_POOL_INDEX_MASK = (1u << SLOT_POOL_INDEX_BITS) - 1u;
		static constexpr uint32_t SLOT_HASH_PROBE_BITS = 3u;
		static constexpr uint32_t SLOT_HASH_PROBE_MASK = ((1u << SLOT_HASH_PROBE_BITS) - 1) << 29u;
		static constexpr uint32_t STRING_HASH_PROBE_BITS = 6u;
		static constexpr uint32_t STRING_HASH_PROBE_MASK = ((1u << STRING_HASH_PROBE_BITS) - 1) << 8;
	private:
		uint32_t slotIndex32;
		uint32_t slotHashProbe3_empty21_slotPoolIndex8;
		StringEntryHeader stringEntryHeader;
		const char* data;
	public:
		inline HashInfo(const std::string_view& string);
		inline const uint32_t GetSlotHashProbe() const;
		inline const StringEntryHeader& GetStringEntryHeader() const;
		inline const char* GetData() const;
		inline const uint32_t GetSlotIndex() const;
		inline const uint8_t GetSlotPoolIndex() const;
	};
	struct Slot
	{
		static constexpr uint32_t SLOT_HASH_PROBE_BITS = 3u;
		static constexpr uint32_t SLOT_HASH_PROBE_MASK = ((1u << SLOT_HASH_PROBE_BITS) - 1) << 29u;
		static constexpr uint32_t STRING_ENTRY_HANDLE_BITS = 29u;
		static constexpr uint32_t STRING_ENTRY_HANDLE_MASK = (1u << STRING_ENTRY_HANDLE_BITS) - 1u;
		static constexpr uint32_t MEMORY_BLOCK_ALIGNED_OFFSET_BITS = 16u;
		static constexpr uint32_t MEMORY_BLOCK_ALIGNED_OFFSET_MASK = (1u << MEMORY_BLOCK_ALIGNED_OFFSET_BITS) - 1u;
		static constexpr uint32_t MEMORY_BLOCK_INDEX_BITS = 13u;
		static constexpr uint32_t MEMORY_BLOCK_INDEX_MASK = (1u << MEMORY_BLOCK_INDEX_BITS) - 1u;
	private:
		uint32_t slotHashProbe3_stringEntryHandle29;
	public:
		inline const bool IsUsed() const;
		inline const bool IsTargetSlot(const HashInfo& hashInfo) const;
		inline const uint32_t GetSlotHashProbe() const;
		inline const StringEntryHandle GetStringEntryHandle() const;
		inline const uint32_t GetStringEntryHandleValue() const;
		inline const uint32_t GetSlotValue() const;
		inline void Load(const uint32_t slotHashProbeValue, const StringEntryHandle& stringEntryHandle);
		inline void Load(const Slot& srcSlot);
	};
	struct SlotPool
	{
		friend class InternedString;
		static constexpr double SLOT_POOL_RESIZE_USAGE_RATE = 0.9;
		static constexpr uint32_t SLOT_POOL_INITIALIZE_SIZE = 256u;
	private:
		uint32_t capcity;
		uint32_t size;
		Slot* slotArray;
		std::mutex mutex;
	public:
		inline Slot& FindUnusedOrTargetSlot(const HashInfo& hashInfo);
		inline Slot& FindUnusedSlot(const HashInfo& hashInfo);
		inline void CheckUsageRateAndResize();
		void Resize();
	};
	struct StringEntryMemoryManager
	{
		friend class InternedString;
		static constexpr uint32_t ALIGN_BYTES = 2u;
		static constexpr uint32_t MAX_MEMORY_BLOCK_SIZE = (1u << 16u) * 2u;
	private:
		uint16_t currentMemoryBlockIndex;
		uint16_t currentMemoryBlockAlignedCursor;
		char* memoryBlockArray[MAX_MEMORY_BLOCK_ARRAY_SIZE];
		std::mutex mutex;
	public:
		inline StringEntry* GetStringEntry(const StringEntryHandle& stringEntryHandle) const;
		inline const StringEntryHandle AllocateStringEntry(const StringEntryHeader& stringEntryHeader, const char* data);
	private:
		inline void CreateNewMemoryBlock();
	};
	static std::array<SlotPool, MAX_SLOT_POOL_ARRAY_SIZE> slotPoolArray;
	static StringEntryMemoryManager stringEntryMemoryManager;
private:
	uint32_t _empty2_isUsed1_StringEntryHandle29;
public:
	inline InternedString()
		: _empty2_isUsed1_StringEntryHandle29(0u)
	{
	}
	~InternedString() = default;
	inline InternedString(const std::string_view& string)
		: _empty2_isUsed1_StringEntryHandle29(MakeInterned(string))
	{
	}
	inline InternedString(const InternedString& internedString)
		: _empty2_isUsed1_StringEntryHandle29(internedString._empty2_isUsed1_StringEntryHandle29)
	{
	}
	inline void operator=(const InternedString& internedString)
	{
		_empty2_isUsed1_StringEntryHandle29 = internedString._empty2_isUsed1_StringEntryHandle29;
	}
	inline InternedString(InternedString&& internedString) noexcept
		: _empty2_isUsed1_StringEntryHandle29(std::move(internedString._empty2_isUsed1_StringEntryHandle29))
	{
	}
	inline void operator=(InternedString&& internedString) noexcept
	{
		_empty2_isUsed1_StringEntryHandle29 = std::move(internedString._empty2_isUsed1_StringEntryHandle29);
	}
	inline bool operator==(const InternedString& r) const
	{
		return _empty2_isUsed1_StringEntryHandle29 == r._empty2_isUsed1_StringEntryHandle29;
	}
	inline bool operator!=(const InternedString& r) const
	{
		return _empty2_isUsed1_StringEntryHandle29 != r._empty2_isUsed1_StringEntryHandle29;
	}
	inline bool operator<(const InternedString& r) const
	{
		return _empty2_isUsed1_StringEntryHandle29 < r._empty2_isUsed1_StringEntryHandle29;
	}
	inline bool operator>(const InternedString& r) const
	{
		return _empty2_isUsed1_StringEntryHandle29 > r._empty2_isUsed1_StringEntryHandle29;
	}

	inline uint16_t Size() const
	{
		const StringEntry* stringEntry = stringEntryMemoryManager.GetStringEntry(StringEntryHandle(_empty2_isUsed1_StringEntryHandle29));
		return stringEntry->GetStringEntryHeader().GetSize();
	}
	std::string_view ToStringView() const;
	std::string ToString() const;
	inline bool IsNULL() const
	{
		return (_empty2_isUsed1_StringEntryHandle29 & _empty2_isUsed1_StringEntryHandle29) == 0u;
	}
	inline uint32_t Value() const
	{
		return _empty2_isUsed1_StringEntryHandle29;
	}
private:
	static const uint32_t MakeInterned(const std::string_view& string);
public:
	static void Initialize();
};

namespace std
{
	template<>
	struct hash<InternedString>
	{
		std::size_t operator()(InternedString const& internedString) const noexcept
		{
			return internedString.Value();
		}
	};
}
