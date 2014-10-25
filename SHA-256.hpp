////////////////////////////////////////////////////////////////////////////
// SHA-256.hpp -- SHA-256 hash generator
// This file is part of MZC3.  See file "ReadMe.txt" and "License.txt".
////////////////////////////////////////////////////////////////////////////
// See FIP180-2 at:
// http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
////////////////////////////////////////////////////////////////////////////
// Thanks, Naoki_I.
////////////////////////////////////////////////////////////////////////////

#ifndef __MZC3_SHA_256_HPP__
#define __MZC3_SHA_256_HPP__

////////////////////////////////////////////////////////////////////////////
// little endian or big endian?

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
    #if defined(_WIN32) || defined(MSDOS) || defined(__i386__)
        #define LITTLE_ENDIAN
    #else
        #define BIG_ENDIAN
    #endif
#endif

#if defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
    #error You lose!
#endif

#ifdef BIG_ENDIAN
    #pragma message("MZC3_SHA-256: Please tell the author whether it works well or not.")
#endif

////////////////////////////////////////////////////////////////////////////
// SHA256_BYTE, SHA256_DWORD, SHA256_QWORD

typedef unsigned char       SHA256_BYTE;
typedef unsigned long       SHA256_DWORD;

#if __cplusplus >= 201103L
    typedef unsigned long long SHA256_QWORD;
#else
    typedef unsigned __int64   SHA256_QWORD;
#endif

////////////////////////////////////////////////////////////////////////////
// MzcHexStringFromBytes

#ifndef MzcHexStringFromBytes
    template <typename T_STRING, typename T_ITER>
    void MzcHexStringFromBytes(T_STRING& str, T_ITER begin_, T_ITER end_) {
        typedef typename T_STRING::value_type T_CHAR;
        static const char s_hex[] = "0123456789abcdef";
        for (str.clear(); begin_ != end_; ++begin_)
        {
            const unsigned char by = *begin_;
            str += static_cast<T_CHAR>(s_hex[by >> 4]);
            str += static_cast<T_CHAR>(s_hex[by & 0x0F]);
        }
    }

    template <typename T_STRING, typename T_CONTAINER>
    inline void MzcHexStringFromBytes(T_STRING& str, const T_CONTAINER& bytes) {
        MzcHexStringFromBytes(str, bytes.begin(), bytes.end());
    }

    #define MzcHexStringFromBytes MzcHexStringFromBytes
#endif  // ndef MzcHexStringFromBytes

////////////////////////////////////////////////////////////////////////////
// MSha256

class MSha256
{
public:
    MSha256() { Init(); }

    // The control order:
    // Init() --> AddData() --> GetHashBinary() or GetHashHexString()

    void Init();

    void AddData(const void* ptr, SHA256_DWORD len);

    template <typename T_ITER>
    void AddData(T_ITER begin_, T_ITER end_) {
        std::size_t len = std::distance(begin_, end_) * sizeof(*begin_);
        AddData(&*begin_, static_cast<SHA256_DWORD>(len));
    }

    void GetHashBinary(void* p32bytes);

    template <typename T_STRING>
    void GetHashHexString(T_STRING& str) {
        char hash[32];
        GetHashBinary(hash);
        MzcHexStringFromBytes(str, hash, hash + 32);
    }

protected:
    SHA256_DWORD m_h[8];
    SHA256_DWORD m_w[64];
    SHA256_DWORD m_iw;
    SHA256_QWORD m_len;

    void UpdateTable();
};

////////////////////////////////////////////////////////////////////////////
// MzcGetSha256Binary and MzcGetSha256HexString

template <typename T_STRING>
inline void MzcGetSha256Binary(
    void *p32bytes, const void *ptr, SHA256_DWORD len)
{
    assert(ptr || len == 0);
    MSha256 sha256;
    sha256.AddData(ptr, len);
    sha256.GetHashBinary(p32bytes);
}

template <typename T_STRING>
inline void MzcGetSha256HexString(
    T_STRING& str, const void *ptr, SHA256_DWORD len)
{
    assert(ptr || len == 0);
    MSha256 sha256;
    sha256.AddData(ptr, len);
    sha256.GetHashHexString(str);
}

template <typename T_STRING, typename T_ITER>
inline void MzcGetSha256HexString(
    T_STRING& str, T_ITER begin_, T_ITER end_)
{
    MSha256 sha256;
    sha256.AddData(begin_, end_);
    sha256.GetHashHexString(str);
}

template <typename T_STRING>
inline void MzcGetSha256HexString(T_STRING& str, const char *psz) {
    assert(psz);
    using namespace std;
    SHA256_DWORD len = static_cast<SHA256_DWORD>(strlen(psz));
    MSha256 sha256;
    sha256.AddData(psz, len);
    sha256.GetHashHexString(str);
}

////////////////////////////////////////////////////////////////////////////

#endif  // ndef __MZC3_SHA_256_HPP__