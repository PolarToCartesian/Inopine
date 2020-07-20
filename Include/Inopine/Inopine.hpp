#pragma once

/*

    +-----------------------------+
    | Inopine Engine Include File |
    +-----------------------------+

    Project........Inopine Engine
    Author.........PolarToCartesian
    Repository.....https://www.github.com/PolarToCartesian/Inopine
    C++ Version....C++20

    Thanks to:
    - Intel Intrinsics Guide: https://software.intel.com/sites/landingpage/IntrinsicsGuide/

    Table Of Contents:
    |--+ C++ Library Includes
    |--+ Non Standard Includes
    |--+ Defines
    |--+ SIMD Wrapper
    |--|--+ SIMD Register Wrapper
    |--|--+ SIMD Operations

*/

// +----------------------+
// | C++ Library Includes |
// +----------------------+

#include <concepts>    // Since C++20
#include <type_traits> // Since C++11 (w/ C++17 Helper Classes)

// +-----------------------+
// | Non Standard Includes |
// +-----------------------+

#ifndef __IE__DISABLE_SIMD

    #include <immintrin.h> // Used for simd types and instructions

#endif // #ifndef __IE__DISABLE_SIMD

// +---------+
// | Defines |
// +---------+

#ifndef __IE__DISABLE_SIMD

    #define __IE__ENABLE_SIMD

#endif // #ifndef __IE__DISABLE_SIMD

/* The "IE" namespace contains all of Inopine Engine's source code in order       */
/* to prevent the clashing of declarations and definitions in the root namespace. */
namespace IE {

    // +--------------+
    // | SIMD Wrapper |
    // +--------------+

    /* The "Internal" namespace contains code that shall be hidden to any user of  */
    /* the engine in order to keep the API simple and hide implementation details. */
    namespace Internal {

        struct SIMD_REGISTER_TYPE_NONE {  };

        // +--------------+     +-----------------------+
        // | SIMD Wrapper | --> | SIMD Register Wrapper |
        // +--------------+     +-----------------------+

#ifdef __IE__DISABLE_SIMD

        template <typename _T>
        using GET_SIMD_VECTOR_REGISTER_TYPE = SIMD_REGISTER_TYPE_NONE;

#else

        template <typename _T>
        using GET_SIMD_VECTOR_REGISTER_TYPE = typename std::conditional_t<std::is_same_v<_T, float>,   __m128,
						                      typename std::conditional_t<std::is_same_v<_T, int32_t>, __m128i,
						                      typename std::conditional_t<std::is_same_v<_T, int16_t>, __m128i,
						                      typename std::conditional_t<std::is_same_v<_T, double>,  __m256d, ::IE::Internal::SIMD_REGISTER_TYPE_NONE>>>>;;

#endif
        
        template <typename _T>
        concept SIMDVectorable = !(std::is_same_v<::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>,
                                                  ::IE::Internal::SIMD_REGISTER_TYPE_NONE>);

        template <typename _T_A, typename _T_B = _T_A>
        static inline constexpr bool CAN_PERFORM_SIMD_VECTOR_OPERATIONS() noexcept
        {
            using _REGISTER_A_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T_A>;
            using _REGISTER_B_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T_B>;

            return !std::is_same_v<_REGISTER_A_TYPE, SIMD_REGISTER_TYPE_NONE>
                &&  std::is_same_v<_REGISTER_A_TYPE, _REGISTER_B_TYPE>;
        }

#ifdef __IE__ENABLE_SIMD

        // +--------------+     +-----------------+
        // | SIMD Wrapper | --> | SIMD Operations |
        // +--------------+     +-----------------+

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDSet(const _T& x, const _T& y, const _T& z, const _T& w) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_set_ps(w, z, y, x);
            else if constexpr (std::is_same_v<_T, int32_t>)
                return _mm_set_epi32(w, z, y, x);
            else if constexpr (std::is_same_v<_T, int16_t>)
                return _mm_set_epi16(0, 0, 0, 0, w, z, y, x);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_set_pd(w, z, y, x);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDAdd(const _VECTOR_REGISTER_TYPE& a, const _VECTOR_REGISTER_TYPE& b) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_add_ps(a, b);
            else if constexpr (std::is_same_v<_T, int32_t>)
                return _mm_add_epi32(a, b);
            else if constexpr (std::is_same_v<_T, int16_t>)
                return _mm_add_epi16(a, b);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_add_pd(a, b);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDSub(const _VECTOR_REGISTER_TYPE& a, const _VECTOR_REGISTER_TYPE& b) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_sub_ps(a, b);
            else if constexpr (std::is_same_v<_T, int32_t>)
                return _mm_sub_epi32(a, b);
            else if constexpr (std::is_same_v<_T, int16_t>)
                return _mm_sub_epi16(a, b);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_sub_pd(a, b);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDMul(const _VECTOR_REGISTER_TYPE& a, const _VECTOR_REGISTER_TYPE& b) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_mul_ps(a, b);
            else if constexpr (std::is_same_v<_T, int32_t>)
                return _mm_mul_epi32(a, b);
            else if constexpr (std::is_same_v<_T, int16_t>)
                return _mm_mullo_epi16(a, b);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_mul_pd(a, b);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDDiv(const _VECTOR_REGISTER_TYPE& a, const _VECTOR_REGISTER_TYPE& b) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_div_ps(a, b);
            else if constexpr (std::is_same_v<_T, int32_t>)
                return _mm_div_epi32(a, b);
            else if constexpr (std::is_same_v<_T, int16_t>)
                return _mm_div_epi16(a, b);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_div_pd(a, b);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline void SIMDStore(_T* src, const _VECTOR_REGISTER_TYPE& sseVec) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                _mm_store_ps(src, sseVec);
            else if constexpr (std::is_same_v<_T, int32_t> || std::is_same_v<_T, int16_t>)
                _mm_store_si128((__m128i*)src, sseVec);
            else if constexpr (std::is_same_v<_T, double>)
                _mm256_store_pd(src, sseVec);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDLoad(_T* src) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_load_ps(src);
            else if constexpr (std::is_same_v<_T, int32_t> || std::is_same_v<_T, int16_t>)
                return _mm_load_si128((__m128i*)src);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_load_pd(src);
        }

        template <::IE::Internal::SIMDVectorable _T, int INDEX, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _T SIMDExtractElement(const _VECTOR_REGISTER_TYPE& sseVector) noexcept
        {
            if constexpr (std::is_same_v<_T, float>) {
                const int l = _mm_extract_ps(sseVector, INDEX);
                return *reinterpret_cast<const _T*>(&l);
            } else if constexpr (std::is_same_v<_T, int32_t>) {
                const int l = _mm_extract_epi16(sseVector, INDEX);
                return *reinterpret_cast<const _T*>(&l);
            } else if constexpr (std::is_same_v<_T, int16_t>) {
                const int l = _mm_extract_epi16(sseVector, INDEX);
                return *reinterpret_cast<const _T*>(&l);
            } else if constexpr (std::is_same_v<_T, double>) {
                if constexpr (INDEX <= 1) {
                    return _mm256_cvtsd_f64(_mm256_shuffle_pd(sseVector, sseVector,
                        _MM_SHUFFLE(INDEX, INDEX, INDEX, INDEX)));
                } else {
                    // This was hard
                    return _mm_cvtsd_f64(_mm_shuffle_pd(_mm256_extractf128_pd(sseVector, 1),
                                                        _mm256_extractf128_pd(sseVector, 1),
                                                        _MM_SHUFFLE(INDEX, INDEX, INDEX, INDEX)));
                }
            }
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::GET_SIMD_VECTOR_REGISTER_TYPE<_T>>
        static inline _T SIMDDotProduct(const _VECTOR_REGISTER_TYPE& a, const _VECTOR_REGISTER_TYPE& b) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return ::IE::Internal::SIMDExtractElement<_T, 0>(_mm_dp_ps(a, b, 0xFF));
            else if constexpr (std::is_same_v<_T, int32_t>)
                return ::IE::Internal::SIMDExtractElement<_T, 0>(_mm_hadd_epi32(::IE::Internal::SIMDMul<_T>(a, b), _mm_setzero_si128()));
            else if constexpr (std::is_same_v<_T, int16_t>)
                return ::IE::Internal::SIMDExtractElement<_T, 0>(_mm_hadd_epi16(_mm_hadd_epi16(::IE::Internal::SIMDMul<_T>(a, b), _mm_setzero_si128()), _mm_setzero_si128()));
            else if constexpr (std::is_same_v<_T, double>)
                return ::IE::Internal::SIMDExtractElement<_T, 0>(_mm256_dp_ps(a, b, 0xFF));
        }

#endif // #ifdef __IE__ENABLE_SIMD

    }; // Internal

}; // IE