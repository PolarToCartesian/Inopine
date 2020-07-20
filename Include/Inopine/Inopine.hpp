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

#include <cmath>
#include <array>       // Since C++11
#include <cstring>
#include <iomanip>
#include <ostream>
#include <iostream>
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

/* EN: The "IE" namespace contains all of Inopine Engine's source code in order          */
/* EN: to prevent the conflicts of declarations and definitions in the root namespace.   */
/* ------------------------------------------------------------------------------------- */
/* FR: L'espace de nom "IE" contient le code source du moteur de rendu graphique Inopiné */
/* FR: afin d'éviter les conflits de noms dans l'espace de nom à la racine.              */
namespace IE {

    template <typename _T>
    concept arithmetic = std::is_arithmetic_v<_T>;

    // +--------------+
    // | SIMD Wrapper |
    // +--------------+

    /* EN: The "Internal" namespace contains code that shall be hidden to any user of  */
    /* EN: the engine in order to keep the API simple and hide implementation details. */
    /* FR: L'espace de nom "Internal" contient le code source du moteur graphique qui  */
    /* FR: doit être caché des utilisateurs pour garder l'interface plus simple.       */
    namespace Internal {

        struct SIMDNoVectorRegister {  };

        // +--------------+     +-----------------------+
        // | SIMD Wrapper | --> | SIMD Register Wrapper |
        // +--------------+     +-----------------------+

        /* EN: The templated "using" definition "SIMDVectorRegister" is used to determine at compile- */
        /* EN: -time the intrinsics' vector register type that corresponds to the template parameter. */
        /* ------------------------------------------------------------------------------------------ */
        /* FR: La définition à modèle de la déclaration "using" de "SIMDVectorRegister" nous permet   */
        /* FR: de determiner le type du registre vectoriel qui correspond à l'argument modèle pendant */
        /* FR: la compilation.                                                                        */

#ifdef __IE__DISABLE_SIMD

        template <typename _T>
        using SIMDVectorRegister = SIMDNoVectorRegister;

#else

        template <typename _T>
        using SIMDVectorRegister = typename std::conditional_t<std::is_same_v<_T, float>,   __m128,
						           typename std::conditional_t<std::is_same_v<_T, int32_t>, __m128i,
						           typename std::conditional_t<std::is_same_v<_T, int16_t>, __m128i,
						           typename std::conditional_t<std::is_same_v<_T, double>,  __m256d, ::IE::Internal::SIMDNoVectorRegister>>>>;;

#endif
        
        /* EN: The concept named "SIMDVectorable" enables us to check wether or not a vector register can be */
        /* EN: Found/Used/Created at compile-time.                                                           */
        /* ------------------------------------------------------------------------------------------------- */
        /* FR: Le concept "SIMDVectorable" nous permet de detecter si le registre vectoriel peut être        */
        /* FR: Trouvé/Utilisé/Créé lors de la compilation.                                                   */

        template <typename _T>
        concept SIMDVectorable = !(std::is_same_v<::IE::Internal::SIMDVectorRegister<_T>,
                                                  ::IE::Internal::SIMDNoVectorRegister>);

        /* EN: The role of the function "CAN_PERFORM_SIMD_VECTOR_OPERATIONS" is the same as the concept "SIMDVectorable" */
        /* EN: but is defined as function "contexpr" function that can be ran at compile-time with two types.           */
        /* ------------------------------------------------------------------------------------------------------------ */
        /* FR: Le rôle de la function "CAN_PERFORM_SIMD_VECTOR_OPERATIONS" est le même que celui du concept             */
        /* FR: "SIMDVectorable" mais elle est définie en tant qu'une fonction "contexpr" qui peut être exécutée lors    */
        /* FR: de la compilation. */

        template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B = _T_A>
        static inline constexpr bool CAN_PERFORM_SIMD_VECTOR_OPERATIONS() noexcept
        {
            using _REGISTER_A_TYPE = ::IE::Internal::SIMDVectorRegister<_T_A>;
            using _REGISTER_B_TYPE = ::IE::Internal::SIMDVectorRegister<_T_B>;

            return !std::is_same_v<_REGISTER_A_TYPE, SIMDNoVectorRegister>
                &&  std::is_same_v<_REGISTER_A_TYPE, _REGISTER_B_TYPE>;
        }

#ifdef __IE__ENABLE_SIMD

        // +--------------+     +-----------------+
        // | SIMD Wrapper | --> | SIMD Operations |
        // +--------------+     +-----------------+

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
        static inline void SIMDStore(_T* src, const _VECTOR_REGISTER_TYPE& sseVec) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                _mm_store_ps(src, sseVec);
            else if constexpr (std::is_same_v<_T, int32_t> || std::is_same_v<_T, int16_t>)
                _mm_store_si128((__m128i*)src, sseVec);
            else if constexpr (std::is_same_v<_T, double>)
                _mm256_store_pd(src, sseVec);
        }

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
        static inline _VECTOR_REGISTER_TYPE SIMDLoad(_T* src) noexcept
        {
            if constexpr (std::is_same_v<_T, float>)
                return _mm_load_ps(src);
            else if constexpr (std::is_same_v<_T, int32_t> || std::is_same_v<_T, int16_t>)
                return _mm_load_si128((__m128i*)src);
            else if constexpr (std::is_same_v<_T, double>)
                return _mm256_load_pd(src);
        }

        template <::IE::Internal::SIMDVectorable _T, int INDEX, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

        template <::IE::Internal::SIMDVectorable _T, typename _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>>
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

    // +--------------+
    // | Math Library |
    // +--------------+

    // +--------------+     +-------------+
    // | Math Library | --> | Vector (4D) |
    // +--------------+     +-------------+

    /* EN: The templated "Vector" class represents a 4 dimensional mathematical vector of arithmitic types. This     */
    /* EN: vector uses compile-time c++ "magic" to use SIMD intrinsics when possible and a "naive method" otherwise. */
    /* EN: It also overloads the common mathematical operators like "+, -, *, and /" to make it easier to use.       */
    /* ------------------------------------------------------------------------------------------------------------- */
    /* FR: La classe modèle "Vector" représente un vecteur mathématique 4 dimensionel de type modèle arithmétiques.  */
    /* FR: Ce vecteur utilise de la "magie" de c++ pour utiliser des instructions SIMD intrinsèques lors qu'il est   */
    /* FR: possible et une méthode "naïve" sinon. Elle "surchage les opérateurs" communs comme "+, -, * et /" afin   */
    /* FR: rendre son utilisation plus simple.                                                                       */

    template <::IE::arithmetic _T = int>
    class Vector {
        using _VECTOR_REGISTER_TYPE = ::IE::Internal::SIMDVectorRegister<_T>;

    public:
        union {
            struct { _T x, y, z, w; };

            _VECTOR_REGISTER_TYPE m_simdRegister;
        };

        // +--------------+     +-------------+     +--------------+
        // | Math Library | --> | Vector (4D) | --> | Constructors |
        // +--------------+     +-------------+     +--------------+

#ifdef __OP__ENABLE_SIMD
        template <::IE::Internal::SIMDVectorable _U = _T>
        Vector(const ::IE::Internal::SIMDVectorRegister<_U>& other) {
            if constexpr (std::is_same_v<_T, _U>) {
                this->m_simdRegister = other;
            } else {
                this->m_simdRegister = ::IE::Internal::SIMDSet<_T>(
                    static_cast<_T>(::IE::Internal::SIMDExtractElement<_U, 0u>(other)),
                    static_cast<_T>(::IE::Internal::SIMDExtractElement<_U, 1u>(other)),
                    static_cast<_T>(::IE::Internal::SIMDExtractElement<_U, 2u>(other)),
                    static_cast<_T>(::IE::Internal::SIMDExtractElement<_U, 3u>(other))
                );
            }
        }
#endif // #ifdef __OP__ENABLE_SIMD 

        Vector(const _T x = 0, const _T y = 0, const _T z = 0, const _T w = 0) noexcept {
            if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T>()) {
#ifdef __OP__ENABLE_SIMD
                this->m_simdRegister = ::IE::Internal::SIMDSet<_T>(x, y, z, w);
#endif // #ifdef __OP__ENABLE_SIMD 
            } else {
                this->x = x; this->y = y; this->z = z; this->w = w;
            }
        }

        // +--------------+     +-------------+     +---------------------------+
        // | Math Library | --> | Vector (4D) | --> | Operators: +=, -=, *=, /= |
        // +--------------+     +-------------+     +---------------------------+

        template <::IE::arithmetic _U>
        inline void operator+=(const Vector<_U>& other) noexcept {
            if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T, _U>()) {
#ifdef __OP__ENABLE_SIMD
                this->m_simdRegister = ::IE::Internal::SIMDAdd(this->m_simdRegister, other.m_simdRegister);
#endif // #ifdef __OP__ENABLE_SIMD 
            } else {
                this->x += other.x; this->y += other.y; this->z += other.z; this->w += other.w;
            }
        }

        template <::IE::arithmetic _U>
        inline void operator-=(const Vector<_U>& other) noexcept {
            if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T, _U>()) {
#ifdef __OP__ENABLE_SIMD
                this->m_simdRegister = ::IE::Internal::SIMDSub(this->m_simdRegister, other.m_simdRegister);
#endif // #ifdef __OP__ENABLE_SIMD 
            } else {
                this->x -= other.x; this->y -= other.y; this->z -= other.z; this->w -= other.w;
            }
        }

        template <::IE::arithmetic _U>
        inline void operator*=(const Vector<_U>& other) noexcept {
            if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T, _U>()) {
#ifdef __OP__ENABLE_SIMD
                this->m_simdRegister = ::IE::Internal::SIMDMul(this->m_simdRegister, other.m_simdRegister);
#endif // #ifdef __OP__ENABLE_SIMD 
            } else {
                this->x *= other.x; this->y *= other.y; this->z *= other.z; this->w *= other.w;
            }
        }

        template <::IE::arithmetic _U>
        inline void operator/=(const Vector<_U>& other) noexcept {
            if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T, _U>()) {
#ifdef __OP__ENABLE_SIMD
                this->m_simdRegister = ::IE::Internal::SIMDDiv(this->m_simdRegister, other.m_simdRegister);
#endif // #ifdef __OP__ENABLE_SIMD 
            } else {
                this->x /= other.x; this->y /= other.y; this->z /= other.z; this->w /= other.w;
            }
        }

        // +--------------+     +-------------+     +--------------------------------+
        // | Math Library | --> | Vector (4D) | --> | Non Static Function Operations |
        // +--------------+     +-------------+     +--------------------------------+

        inline float GetLength() const noexcept {
            if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T>()) {
#ifndef __WEISS__DISABLE_SIMD
                return std::sqrt(::IE::Internal::SIMDDotProduct<_T>(this->m_simdRegister, this->m_simdRegister));
#endif // #ifndef __WEISS__DISABLE_SIMD
			} else {
				return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
			}
        }

        inline void Normalize() noexcept {
            this->operator/=(this->GetLength());
        }

        // +--------------+     +-------------+     +----------------------------+
        // | Math Library | --> | Vector (4D) | --> | Static Function Operations |
        // +--------------+     +-------------+     +----------------------------+

        template <::IE::arithmetic _T_A>
		inline static ::IE::Vector<_T_A> Normalized(const ::IE::Vector<_T_A>& vec) noexcept
		{
			return vec / vec.GetLength();
		}

		template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
		inline static auto CrossProduct3D(const ::IE::Vector<_T_A>& vecA, const ::IE::Vector<_T_B>& vecB) noexcept
			-> ::IE::Vector<decltype(vecA.x + vecB.x)>
		{
			if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T_A, _T_B>()) {
#ifndef __WEISS__DISABLE_SIMD
				const ::IE::Vector<_T_A> tempA1(vecA.y, vecA.z, vecA.x, 0);
				const ::IE::Vector<_T_A> tempB1(vecB.z, vecB.x, vecB.y, 0);
				const ::IE::Vector<_T_A> tempA2(vecA.z, vecA.x, vecA.y, 0);
				const ::IE::Vector<_T_A> tempB2(vecB.y, vecB.z, vecB.x, 0);

				return tempA1 * tempB1 - tempA2 * tempB2;
#endif // #ifndef __WEISS__DISABLE_SIMD
			} else {
				return ::IE::Vector<decltype(vecA.x + vecB.x)>(
					vecA.y * vecB.z - vecA.z * vecB.y,
					vecA.z * vecB.x - vecA.x * vecB.z,
					vecA.x * vecB.y - vecA.y * vecB.x,
					0
				);
			}
		}

		template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
		static inline auto DotProduct(const ::IE::Vector<_T_A>& a, const ::IE::Vector<_T_B>& b) noexcept
			-> decltype(a.x + b.x)
		{
			if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T_A, _T_B>()) {
#ifndef __WEISS__DISABLE_SIMD
				return ::IE::Internal::SIMDDotProduct<_T_A>(a.m_simdRegister, b.m_simdRegister);
#endif // #ifndef __WEISS__DISABLE_SIMD
			} else {
				return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			}
		}

		template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
		static inline auto GetReflected(const ::IE::Vector<_T_A>& in, const ::IE::Vector<_T_B>& normal) noexcept
			-> ::IE::Vector<decltype(in.x + normal.x)>
		{
			return in - normal * 2 * ::IE::Vector<>::DotProduct(in, normal);
		}
    };

    // +--------------+     +-------------+     +--------------------------------+
    // | Math Library | --> | Vector (4D) | --> | Operators: +, -, *, /          |
    // +--------------+     +-------------+     +--------------------------------+

    template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
    inline auto operator+(const ::IE::Vector<_T_A>& a, const ::IE::Vector<_T_B>& b) noexcept
        -> Vector<decltype(a.x + b.x)>
    {
        using _RETURN_VECTOR_TYPE = Vector<decltype(a.x + b.x)>;

        if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T_A, _T_B>()) {
#ifdef __OP__ENABLE_SIMD
            return _RETURN_VECTOR_TYPE(::IE::Internal::SIMDAdd<_T_A>(a.m_simdRegister, b.m_simdRegister));
#endif // #ifdef __OP__ENABLE_SIMD 
        } else {
            return _RETURN_VECTOR_TYPE(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }
    }

    template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
    inline auto operator-(const ::IE::Vector<_T_A>& a, const ::IE::Vector<_T_B>& b) noexcept
        -> Vector<decltype(a.x - b.x)>
    {
        using _RETURN_VECTOR_TYPE = Vector<decltype(a.x - b.x)>;

        if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T_A, _T_B>()) {
#ifdef __OP__ENABLE_SIMD
            return _RETURN_VECTOR_TYPE(::IE::Internal::SIMDSub<_T_A>(a.m_simdRegister, b.m_simdRegister));
#endif // #ifdef __OP__ENABLE_SIMD 
        } else {
            return _RETURN_VECTOR_TYPE(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }
    }

    template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
    inline auto operator*(const ::IE::Vector<_T_A>& a, const ::IE::Vector<_T_B>& b) noexcept
        -> Vector<decltype(a.x * b.x)>
    {
        using _RETURN_VECTOR_TYPE = Vector<decltype(a.x * b.x)>;

        if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T_A, _T_B>()) {
#ifdef __OP__ENABLE_SIMD
            return _RETURN_VECTOR_TYPE(::IE::Internal::SIMDMul<_T_A>(a.m_simdRegister, b.m_simdRegister));
#endif // #ifdef __OP__ENABLE_SIMD 
        } else {
            return _RETURN_VECTOR_TYPE(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }
    }

    template <::IE::arithmetic _T_A, ::IE::arithmetic _T_B>
    inline auto operator/(const ::IE::Vector<_T_A>& a, const ::IE::Vector<_T_B>& b) noexcept
        -> Vector<decltype(a.x / b.x)>
    {
        using _RETURN_VECTOR_TYPE = Vector<decltype(a.x * b.x)>;

        if constexpr (::IE::Internal::CAN_PERFORM_SIMD_VECTOR_OPERATIONS<_T_A, _T_B>()) {
#ifdef __OP__ENABLE_SIMD
            return _RETURN_VECTOR_TYPE(::IE::Internal::SIMDDiv<_T_A>(a.m_simdRegister, b.m_simdRegister));
#endif // #ifdef __OP__ENABLE_SIMD 
        } else {
            return _RETURN_VECTOR_TYPE(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
        }
    }

    template <::IE::arithmetic _T>
	inline std::ostream& operator<<(std::ostream& stream, const ::IE::Vector<_T>& vec) noexcept
	{
		stream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";

		return stream;
	}

    // +--------------+     +-------------+     +------------------------+
    // | Math Library | --> | Vector (4D) | --> | Useful vector Typedefs |
    // +--------------+     +-------------+     +------------------------+

    typedef ::IE::Vector<int8_t>  Veci8;
	typedef ::IE::Vector<int16_t> Veci16;
	typedef ::IE::Vector<int32_t> Veci32;

	typedef ::IE::Vector<uint8_t>  Vecu8;
	typedef ::IE::Vector<uint16_t> Vecu16;
	typedef ::IE::Vector<uint32_t> Vecu32;

	typedef ::IE::Vector<std::enable_if_t<std::numeric_limits<float>::is_iec559, float>>   Vecf32;
	typedef ::IE::Vector<std::enable_if_t<std::numeric_limits<double>::is_iec559, double>> Vecd64;

	typedef ::IE::Vecf32 Colorf32;
	typedef ::IE::Vecu8  Coloru8;

    // +--------------+     +--------------+
    // | Math Library | --> | Matrix (4x4) |
    // +--------------+     +--------------+

    /* [EN] Represents a row-major 4x4 matrix of "_T" values as shown below:
     * [FR] Représente une matrice 4x4 de valeurs de types "_T" d'ordre de rangée principal comme:
	 * |-------------------|-------------------|-------------------|-------------------|
	 * | _T m00 (index 0)  | _T m01 (index 1)  | _T m02 (index 2)  | _T m03 (index 3)  |
	 * | _T m10 (index 4)  | _T m11 (index 5)  | _T m12 (index 6)  | _T m13 (index 7)  |
	 * | _T m20 (index 8)  | _T m21 (index 9)  | _T m22 (index 10) | _T m23 (index 11) |
	 * | _T m30 (index 12) | _T m31 (index 13) | _T m32 (index 14) | _T m33 (index 15) |
	 * |-------------------|-------------------|-------------------|-------------------|
	 */

	template <::IE::arithmetic _T>
	class Matrix
	{
	private:
		_T m[16] = { 0 };

	public:
        // +--------------+     +--------------+     +--------------+
        // | Math Library | --> | Matrix (4x4) | --> | Constructors |
        // +--------------+     +--------------+     +--------------+

		Matrix() = default;

		inline Matrix(const _T* buff)                 noexcept { std::memcpy(this->m, buff, sizeof(_T) * 16u);       }
		inline Matrix(const std::array<_T, 16u>& arr) noexcept { std::memcpy(this->m, arr.data(), sizeof(_T) * 16u); }
		inline Matrix(const ::IE::Matrix<_T>& other)  noexcept { std::memcpy(this->m, other.m, sizeof(_T) * 16u);    }

        // +--------------+     +--------------+     +----------------------+
        // | Math Library | --> | Matrix (4x4) | --> | Overloaded Operators |
        // +--------------+     +--------------+     +----------------------+

		inline       _T& operator[](const size_t i)       noexcept { return this->m[i]; }
		inline const _T& operator[](const size_t i) const noexcept { return this->m[i]; }

		inline       _T& operator()(const size_t r, const size_t c)       noexcept { return this->m[r * 4u + c]; }
		inline const _T& operator()(const size_t r, const size_t c) const noexcept { return this->m[r * 4u + c]; }

        // +--------------+     +--------------+     +------------------+
        // | Math Library | --> | Matrix (4x4) | --> | Static Functions |
        // +--------------+     +--------------+     +------------------+

		static inline ::IE::Matrix<_T> MakeZeros()    noexcept { return ::IE::Matrix<_T>(); }

		static inline ::IE::Matrix<_T> MakeIdentity() noexcept {
			return ::IE::Matrix<_T>(std::array<_T, 16u>{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			});
		}

		static inline ::IE::Matrix<_T> MakeRotationX(const float radX = 0) noexcept
		{
			const _T sinX = std::sin(radX);
			const _T cosX = std::cos(radX);

			return ::IE::Matrix<_T>(std::array<_T, 16u>{
				1, 0,    0,     0,
				0, cosX, -sinX, 0,
				0, sinX, cosX,  0,
				0, 0,    0,     1
			});
		}

		static inline ::IE::Matrix<_T> MakeRotationY(const float radY = 0) noexcept
		{
			const _T sinY = std::sin(radY);
			const _T cosY = std::cos(radY);

			return ::IE::Matrix<_T>(std::array<_T, 16u>{
				cosY,  0, sinY, 0,
				0,     1, 0,    0,
				-sinY, 0, cosY, 0,
				0,     0, 0,    1
			});
		}

		static inline ::IE::Matrix<_T> MakeRotationZ(const float radZ = 0) noexcept
		{
			const _T sinZ = std::sin(radZ);
			const _T cosZ = std::cos(radZ);

			return ::IE::Matrix<_T>(std::array<_T, 16u>{
				cosZ, -sinZ, 0, 0,
				sinZ, cosZ,  0, 0,
				0,    0,     1, 0,
				0,    0,     0, 1
			});
		}

		static inline ::IE::Matrix<_T> MakeRotation(const float radX = 0, const float radY = 0, const float radZ = 0) noexcept
		{
			return MakeRotationX(radX) * MakeRotationY(radY) * MakeRotationZ(radZ);
		}

		static inline ::IE::Matrix<_T> MakeRotation(const Vector<_T>& radians) noexcept
		{
			return Matrix<_T>::MakeRotation(radians.x, radians.y, radians.z);
		}

		static inline ::IE::Matrix<_T> MakeTranslation(const float x, const float y, const float z) noexcept
		{
			return Matrix<_T>(std::array<_T, 16u>{
				1,  0,  0,  0,
				0,  1,  0,  0,
				0,  0,  1,  0,
				-x, -y, -z, 1,
			});
		}

		static inline ::IE::Matrix<_T> MakeTranslation(const Vector<_T>& translation) noexcept
		{
			return Matrix<_T>::MakeTranslation(translation.x, translation.y, translation.z);
		}

		static inline ::IE::Matrix<_T> MakePerspective(const float zNear, const float zFar, const float fovRad, const float aspectRatio)
		{
			return Matrix<_T>(std::array<_T, 16u>{
				aspectRatio * fovRad, 0,      0,                                0,
				0,                    fovRad, 0,                                0,
				0,                    0,      zFar / (zFar - zNear),            1,
				0,                    0,      (-zFar * zNear) / (zFar - zNear), 1,
			});
		}

		static inline ::IE::Matrix<_T> MakeScaling(const float scaleX = 1.0f, const float scaleY = 1.0f, const float scaleZ = 1.0f, const float scaleW = 1.0f) noexcept
		{
			return Matrix<_T>(std::array<_T, 16u>{
				scaleX, 0, 0, 0,
				0, scaleY, 0, 0,
				0, 0, scaleZ, 0,
				0, 0, 0, scaleW
			});
		}

		static inline ::IE::Matrix<_T> MakeScaling(const Vecf32& scale) noexcept
		{
			return Matrix<_T>::MakeScaling(scale.x, scale.y, scale.z, scale.w);
		}

		static inline ::IE::Matrix<_T> MakeTransposed(const Matrix<_T>& mat) noexcept
		{
			return Matrix<_T>(std::array<_T, 16u>{
				mat(0, 0), mat(1, 0), mat(2, 0), mat(3, 0),
				mat(0, 1), mat(1, 1), mat(2, 1), mat(3, 1),
				mat(0, 2), mat(1, 2), mat(2, 2), mat(3, 2),
				mat(0, 3), mat(1, 3), mat(2, 3), mat(3, 3)
			});
		}

		// Thanks to: https://stackoverflow.com/questions/349050/calculating-a-lookat-matrix
		static inline ::IE::Matrix<_T> MakeLookAt(const ::IE::Vector<_T>& cameraPosition, const ::IE::Vector<_T>& focalPoint, const ::IE::Vector<_T>& upDirection) noexcept
		{
			const ::IE::Vector<_T> zaxis = ::IE::Vector<_T>::Normalized(focalPoint - cameraPosition);
			const ::IE::Vector<_T> xaxis = ::IE::Vector<_T>::Normalized(Vector<_T>::CrossProduct3D(upDirection, zaxis));
			const ::IE::Vector<_T> yaxis = ::IE::Vector<_T>::CrossProduct3D(zaxis, xaxis);

			const _T m30 = -::IE::Vector<_T>::DotProduct(xaxis, cameraPosition);
			const _T m31 = -::IE::Vector<_T>::DotProduct(yaxis, cameraPosition);
			const _T m32 = -::IE::Vector<_T>::DotProduct(zaxis, cameraPosition);

			return Matrix<_T>(std::array<_T, 16u>{
				xaxis.x, yaxis.x, zaxis.x, 0,
				xaxis.y, yaxis.y, zaxis.y, 0,
				xaxis.z, yaxis.z, zaxis.z, 0,
				m30,     m31,     m32,     1
			});
		}
	};

    // +--------------+     +--------------+     +--------------+
    // | Math Library | --> | Matrix (4x4) | --> | Operators: * |
    // +--------------+     +--------------+     +--------------+

	template <::IE::arithmetic _T, ::IE::arithmetic _T_2>
	inline auto operator*(const ::IE::Matrix<_T>& matA, const ::IE::Matrix<_T_2>& matB) noexcept
		-> ::IE::Matrix<decltype(matA[0] + matB[0])>
	{
		::IE::Matrix<decltype(matA[0] + matB[0])> matResult;

		for (size_t r = 0u; r < 4u; r++) {
			for (size_t c = 0u; c < 4u; c++) {
				const ::IE::Vector<_T>   vecA = ::IE::Vector<_T>(matA(r, 0), matA(r, 1), matA(r, 2), matA(r, 3));
				const ::IE::Vector<_T_2> vecB = ::IE::Vector<_T_2>(matB(0, c), matB(1, c), matB(2, c), matB(3, c));

				matResult(r, c) = ::IE::Vector<>::DotProduct(vecA, vecB);
			}
		}

		return matResult;
	}

    /*
	 * Multiplies a 1x4 vector with a 4x4 matrix like shown below :
								    |----------------------------------------|
								    | _T_2 m00; _T_2 m01; _T_2 m02; _T_2 m03 |
								    | _T_2 m10; _T_2 m11; _T_2 m12; _T_2 m13 |
	   |------------------------|   | _T_2 m20; _T_2 m21; _T_2 m22; _T_2 m23 |   |----------------------------|
	   | _T x; _T y; _T z; _T w | * | _T_2 m30; _T_2 m31; _T_2 m32; _T_2 m33 | = | decltype(x+m00) x, y, z, w |
	   |------------------------|   |----------------------------------------|   |----------------------------|
	 */

	template <::IE::arithmetic _T, ::IE::arithmetic _T_2>
	inline auto operator*(const ::IE::Vector<_T>& vec, const ::IE::Matrix<_T_2>& mat) noexcept
		-> ::IE::Vector<decltype(vec.x + mat[0])>
	{
		return ::IE::Vector<decltype(vec.x + mat[0])>(
			::IE::Vector<>::DotProduct(vec, Vector<_T_2>(mat(0, 0), mat(1, 0), mat(2, 0), mat(3, 0))),
			::IE::Vector<>::DotProduct(vec, Vector<_T_2>(mat(0, 1), mat(1, 1), mat(2, 1), mat(3, 1))),
			::IE::Vector<>::DotProduct(vec, Vector<_T_2>(mat(0, 2), mat(1, 2), mat(2, 2), mat(3, 2))),
			::IE::Vector<>::DotProduct(vec, Vector<_T_2>(mat(0, 3), mat(1, 3), mat(2, 3), mat(3, 3)))
		);
	}

    // +--------------+     +--------------+     +----------+
    // | Math Library | --> | Matrix (4x4) | --> | Printing |
    // +--------------+     +--------------+     +----------+

	template <::IE::arithmetic _T>
	inline std::ostream& operator<<(std::ostream& stream, const ::IE::Matrix<_T>& mat) noexcept
	{
		constexpr const size_t MAX_DIGITS = 5u;

		stream << std::setprecision(MAX_DIGITS) << '|';

		for (size_t i = 0u; i < 4u * ((MAX_DIGITS + 1u) + 1u) + 1u; i++)
			stream << '-';

		stream << "|\n";
		for (size_t r = 0u; r < 4u; r++) {
			stream << "| ";

			for (size_t c = 0u; c < 4u; c++)
				stream << std::setw(MAX_DIGITS + 1u) << mat(r, c) << ' ';

			stream << "|\n";
		}

		stream << '|';

		for (size_t i = 0u; i < 4u * ((MAX_DIGITS + 1u) + 1u) + 1u; i++)
			stream << '-';

		stream << "|\n";

		return stream;
	}

    // +--------------+     +--------------+     +-----------------+
    // | Math Library | --> | Matrix (4x4) | --> | Useful Typedefs |
    // +--------------+     +--------------+     +-----------------+

	typedef ::IE::Matrix<std::enable_if_t<std::numeric_limits<float>::is_iec559, float>> Matf32;

}; // IE