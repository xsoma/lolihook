#pragma once

namespace util {
	Address copy(Address dst, Address src, size_t size);

	// memset
	Address set(Address dst, uint8_t val, size_t size);


	template< typename o = void*, typename i = void* >
	o force_cast(i in);

	template < typename t = Address >
	t get_method(Address this_ptr, size_t index);
	

	// get base ptr ( EBP (x86_32) / RBP (x86_64) ).
	uintptr_t GetBasePointer();

	// wide -> multi-byte
	std::string WideToMultiByte(const std::wstring& str);

	// multi-byte -> wide
	std::wstring MultiByteToWide(const std::string& str);


	Address PatternScan(uintptr_t mod, const char* sig, size_t size);

	std::string BytesToIDAPattern(byte* bytes, size_t size);

	std::vector<uintptr_t> GetXRefsTo(uintptr_t address, uintptr_t start, uintptr_t size);
	
	bool GetSectionInfo(uintptr_t base_address, const char* section_name, uintptr_t& section_start, uintptr_t& section_size);

	



}