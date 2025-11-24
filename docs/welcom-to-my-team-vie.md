# 1 - XIN CHÀO

Cảm ơn bạn đã tham gia vào nhóm của mình. Dưới đây là phần giới thiệu về các quy ước trong viết mã.

# 2 - CÁC QUY ƯỚC
## 2.1 - Chú thích (Comment)

Việc chú thích sẽ làm mất thời gian trong lúc viết mã (coding) của bạn hơn, nhưng đổi lại bạn sẽ dễ hiểu mã của bạn hơn sau này, hoặc một lúc sau đó. Ngoài ra còn giúp các bạn chung nhóm khác hiểu khái quát về đoạn mã.
	
### 2.1.1 - Chú thích biến, struct, enum, union, macro

Mình thường sử dụng chú thích kiểu Doxygen cho các hàm, và kiểu chú thích thông thường cho các biến.

**NOTE:**

- Đối với chú thích bình thường `//` hay `///` là như nhau!
- Hãy viết hàm trước, sau đó chú thích sau nếu hàm **không quá phức tạp**.
- Hãy chú thích trường hợp trước, và viết code sau nếu **hàm phức tạp**.

**Ví dụ 2.1.1.1: Chú thích kiểu Doxygen cho biến, các thuộc tính của struct, enum, ...**

```C

/// BIẾN TOÀN CỤC ///////////////////////////////////////////////////////////////////////

	/// The buffer array pointer
	int* buffPtr;
	/// Size of buffer
	int buffSize;

	int* buffPtr;				/// The buffer array pointer
	int buffSize;				/// Size of buffer

/// ĐỊNH NGHĨA STRUCT ///////////////////////////////////////////////////////////////////

	/// Config object
	typedef struct config_t {
		/// Path to gz file
		path_t      configPath;
		/// Number of entries
		int32_t     configNum;
		/// Entry type
		uint8_t*    configType;
		/// Entry label
		label_t*    configLabel;
		/// Entry value
		txtVal_t*   configValue;
	} config_t;

	/// Config object
	typedef struct config_t {
		path_t      configPath;                     /// Path to gz file
		int32_t     configNum;                      /// Number of entries
		uint8_t*    configType;                     /// Entry type
		label_t*    configLabel;                    /// Entry label
		txtVal_t*   configValue;                    /// Entry value
	} config_t;

/// ĐỊNH NGHĨA ENUM /////////////////////////////////////////////////////////////////////

	/// Return types of config status
	enum CONFIG_STATUS_RETURN_TYPE {
		CONFIG_STATUS_OKE = 0,              /// No error
		CONFIG_STATUS_NULL_ERROR,           /// Try to access to NULL pointer
		CONFIG_STATUS_ALLOC_FAILED,         /// Can not allocate a memory block
		CONFIG_STATUS_UNKNOWN_ERROR,        /// Just occured an error
		CONFIG_STATUS_FILE_NOT_FOUND,       /// Can not open the file
		CONFIG_STATUS_PARAMETER_ERROR,      /// The param is invalid
		CONFIG_STATUS_FILE_NOT_EXISTED,     /// File not existed
	};

	/// Return types of config status
	enum CONFIG_STATUS_RETURN_TYPE {
		/// No error
		CONFIG_STATUS_OKE = 0,
		
		CONFIG_STATUS_NULL_ERROR,
		/// Can not allocate a memory block
		CONFIG_STATUS_ALLOC_FAILED,
		/// Just occured an error
		CONFIG_STATUS_UNKNOWN_ERROR,
		/// Can not open the file
		CONFIG_STATUS_FILE_NOT_FOUND,
		/// The param is invalid
		CONFIG_STATUS_PARAMETER_ERROR,
		/// File not existed
		CONFIG_STATUS_FILE_NOT_EXISTED,
	};
	
/// MACRO ///////////////////////////////////////////////////////////////////////////////
	
	/// 29 first bytes for config prefix + 1 byte '\n'
	#define CONFIG_PREFIX        "__NGXXFUS__BEGIN_OF_CONFIG__"
	/// 29 first bytes for config size prefix + number + 1 byte '\n'
	#define CONFIG_SIZE_PREFIX   "__NGXXFUS__NUMBER_OF_CONFIG_"
	/// 29 last bytes for config suffix
	#define CONFIG_SUFFIX        "__NGXXFUS__END_OF_CONFIG____"

```

### 2.1.2 - Chú thích hàm, thủ tục, macro

**Ví dụ 2.1.2.1: Chú thích kiểu Doxygen cho hàm**

**NOTE:**
- Ngoài dùng `///` cho từng dòng trong chú thích kiểu Doxygen, còn có thể gom hết vào một `/**/` (chú thích đoạn, xem phần chú thích cho macro)
- Có thể sử dụng chú thích `//comment` hay `/*comment*/` cho hàm.

```C
/// THỦ TỤC (VOID) //////////////////////////////////////////////////////////////////////

	/// @brief Make a simple random function
	/// @param aParam0 Core of the simple random function 
	/// @param aParam1 The pointer to where stores the random result
	void aFunction(int aParam0, int* aParam1){
		static int coefficient = 19;
		coefficient = ((coefficient*aParam0/17)<<2) +  ((coefficient*aParam0/37)>>2);
		*aParam1 = coefficient;
	}

/// HÀM (RETURN INT, ...) ///////////////////////////////////////////////////////////////

	/// @brief Make a simple random function
	/// @param aParam Core of the simple random function 
	/// @return The new random value
	int aFunction(int aParam){
		static int coefficient = 19;
		coefficient = ((coefficient*aParam/17)<<2) +  ((coefficient*aParam/37)>>2);
		return coefficient;
	}

/// MACRO ///////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief Reverse loop macro for iterating downward with a specified type.
	 * @param type The data type of the loop variable.
	 * @param i The loop variable.
	 * @param a The starting value (inclusive).
	 * @param b The ending value (exclusive).
	 * @example REVT(int, i, 5, 0) iterates i from 5 to 1.
	 */
	#ifndef REVT
		#define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))
	#endif
	
```

Ngoài ra cũng có thể dùng chú thích thường để chú thích cho hàm

```C
	/// Build filename: /tmp/clipboard/historydata_<id>_<datatype>.<ext> or in IMG_DIR for images
	void build_data_filename(char *buf, size_t buflen, unsigned int id, const char *datatype, const char *ext);
	
	/// If file exists at path, unlink it
	void unlink_if_exists(const char *path);
```

<!-- **Result (in VSCode):**

Khi giữ chuột lên các hàm, biến, macro, ... đã được ghi chú, sẽ hiện thông tin ghi chú, điều này sẽ giúp bạn nắm bắt các biến tốt hơn và lập trình tốt hơn.

![image](imgs/image0.png)

![image](imgs/image1.png) -->

## 2.2 - Đặt tên hàm và biến

Để tiện lợi trong quá trình phát triển và bảo trì mã nguồn, chúng ta phải thống nhất một số quy ước về cách đặt tên biến. Trong các ngôn ngữ như C/C++/Python, các ký tự hoa và thường là phân biệt nhau!

### 2.2.1 - Quy tắc đặt tên chung

- Viết thường đối với tên biến có một từ. VD: `text`, `buff`
- Đối với các biến dạng private (C++/class), cục bộ/nội bộ (local) thì nên thêm `__` vào làm tiền tố.
- Từ (word) đầu tiên viết thường, các từ còn lại viết hoa chữ cái đầu. VD: `textSize`, `buffSize`
- Viết hoa ký tự viết tắt
- Đối với số thì thường đặt cuối
- IN HOA toàn bộ đối với macro hằng, enum 

**Ví dụ 2.2.1.1: Ví dụ đặt tên biến và hàm theo quy tắc chung**

```C
/// BIẾN NỘI BỘ /////////////////////////////////////////////////////////////////////////

	pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

	void __entryCriticalSection(pthread_mutex_t* mutex) {
		int rc = pthread_mutex_lock(mutex);
		if (rc) {
		    fprintf(stderr, "[err] [critical section] Lock failed: %d\n", rc);
		}
	}

	void __exitCriticalSection(pthread_mutex_t* mutex) {
		int rc = pthread_mutex_unlock(mutex);
		if (rc) {
		    fprintf(stderr, "[err] [critical section] Unlock failed: %d\n", rc);
		}
	}
	
/// BIẾN THÔNG THƯỜNG ///////////////////////////////////////////////////////////////////
	
	#define fMask(f)            __mask64(f)
	#define fInvMask(f)         __inv_mask64(f)
	#define hasFlag(f)          & fMask(f)
	#define setFlag(f)          |= fMask(f)
	#define clrFlag(f)          &= fInvMask(f)

	extern windowContext_t *    oscWin;
	extern pthread_mutex_t      sdlMutex;                   /// Mutex lock for SDL operations (thread-safety)
	extern pthread_mutex_t      scrBufMutex;                /// Mutex lock for screen buffer
	extern const xy_t           screenW;                    /// Y
	extern const xy_t           screenH;                    /// X
	extern color_t *            screenBuffer;				/// Screen buffer
	
/// MACRO  & ENUM ///////////////////////////////////////////////////////////////////////
	
	#define CONFIG_PATH_SIZE     256
	#define CONFIG_LABEL_SIZE     64
	#define CONFIG_VALUE_SIZE    256

	/// 29 first bytes for config prefix + 1 byte '\n'
	#define CONFIG_PREFIX        "__NGXXFUS__BEGIN_OF_CONFIG__"
	/// 29 first bytes for config size prefix + number + 1 byte '\n'
	#define CONFIG_SIZE_PREFIX   "__NGXXFUS__NUMBER_OF_CONFIG_"
	/// 29 last bytes for config suffix
	#define CONFIG_SUFFIX        "__NGXXFUS__END_OF_CONFIG____"
	
	/// Convert variable name to string
	#define STRINGIFY(x) #x
	#define NAME2STR(x) STRINGIFY(x)

	/// Return types of config status
	enum CONFIG_STATUS_RETURN_TYPE {
		CONFIG_STATUS_OKE = 0,              /// No error
		CONFIG_STATUS_NULL_ERROR,           /// Try to access to NULL pointer
		CONFIG_STATUS_ALLOC_FAILED,         /// Can not allocate a memory block
		CONFIG_STATUS_UNKNOWN_ERROR,        /// Just occured an error
		CONFIG_STATUS_FILE_NOT_FOUND,       /// Can not open the file
		CONFIG_STATUS_PARAMETER_ERROR,      /// The param is invalid
		CONFIG_STATUS_FILE_NOT_EXISTED,     /// File not existed
	};
	
```

Lưu ý: Không viết tắt quá nhiều, sẽ khó đọc.

### 2.2.2 - Quy tắc đặt tên bất thường

- Thêm tiền tố vào để đánh dấu các hàm, biến này cùng thuộc một nhóm.
- Có thể thêm tiền tố `<GROUP>__` dạng `<GROUP>__TextTextNumber` để đánh dấu tên hàm và biến thuộc một nhóm, VD: `SDL_Rect`, `SDL_Texture`.
- Thêm hậu tố `_t` để đánh dấu là kiểu dữ liệu được định nghĩa.

**Ví dụ 2.2.2.1: Ví dụ đặt tên biến và hàm theo quy tắc bất thường**

```C

	typedef int8_t      confStatus_t;
	typedef int32_t     numVal_t;
	typedef char *      txtVal_t;
	typedef char *      label_t;
	typedef char *      path_t;

	/// Config entry type
	enum CONFIG_TYPE {
		CONFIG_TYPE_NOT_SET = 0,
		CONFIG_TYPE_TEXT,
		CONFIG_TYPE_NUMBER,
	};

	/// Config object
	typedef struct config_t {
		path_t      configPath;                     /// Path to gz file
		int32_t     configNum;                      /// Number of entries
		uint8_t*    configType;                     /// Entry type
		label_t*    configLabel;                    /// Entry label
		txtVal_t*   configValue;                    /// Entry value
	} config_t;

```

## 2.3 - Trường tượng hóa/Chuẩn hóa kiểu dữ liệu

Trừu tượng hóa cho phép chúng ta tập trung vào những khái niệm chung, thay vì bị chi phối bởi quá nhiều chi tiết nhỏ.
Chuẩn hóa giúp mã nguồn dễ đọc, dễ bảo trì hơn và làm cho luồng tư duy trong quá trình phát triển phần mềm trở nên rõ ràng, mạch lạc hơn.

- Trừu tượng hóa: đặt tên kiểu dữ liệu sao cho phù hợp với ý nghĩa và mục đích sử dụng, thay vì dùng trực tiếp kiểu dữ liệu nguyên thủy.
- Chuẩn hóa: đảm bảo sự thống nhất trong cách sử dụng dữ liệu. Khi có sự thay đổi về định nghĩa kiểu dữ liệu, hệ thống sẽ ít bị ảnh hưởng (hoặc không bị ảnh hưởng) nhờ việc dùng các kiểu đã được chuẩn hóa thay thế cho kiểu gốc.
- Kể cả việc trả về trạng thái lỗi, khi ta chuẩn hóa thông qua các ENUM, ta sẽ dễ để gỡ lỗi (debug) hơn.

**Ví dụ 2.3.1: Ví dụ về trường tượng hóa/chuẩn hóa kiểu dữ liệu**

```C
/// KHÔNG CHUẨN HÓA /////////////////////////////////////////////////////////////////////

	...

	/// Config object
	typedef struct config_t {
		char*       configPath;                     ///< Path to gz file
		int32_t     configNum;                      ///< Number of entries
		uint8_t*    configType;                     ///< Entry type
		char**      configLabel;                    ///< Entry label
		char**      configValue;                    ///< Entry value
	} config_t;

	/// @brief Convert from int to string
	void intToStr(char* result, size_t resultSize, int32_t value);

	/// @brief Convert an integer value to string
	/// @param result The buffer to store the result string
	/// @param resultSize The size of the result buffer
	/// @param value The integer value to convert
	void intToStr(char* result, size_t resultSize, int32_t value);

	/// @brief Initialize configuration from a GZipped file
	/// @param confPtr Pointer to a config_t* that will store the created config
	/// @param gzPath Path to the gzipped config file
	/// @return Status of the operation (int8_t)
	int8_t initialConfigFromGZFile(config_t ** confPtr, const char* gzPath);

	/// @brief Create a configuration object
	/// @param confPtr Pointer to a config_t* that will store the created config
	/// @param path Path to the config file
	/// @param confNum Number of configuration entries
	/// @return Status of the operation (int8_t)
	int8_t createConfig(config_t ** confPtr, const char* path, int32_t confNum);

	/// @brief Delete a configuration object and free all memory
	/// @param confPtr Pointer to the config_t* to delete
	void deleteConfig(config_t ** confPtr);

	...

/// CHUẨN HÓA ///////////////////////////////////////////////////////////////////////////
	...

	typedef int8_t      confStatus_t;
	typedef int32_t     numVal_t;
	typedef char *      txtVal_t;
	typedef char *      label_t;
	typedef char *      path_t;

	...

	/// Config object
	typedef struct config_t {
		path_t      configPath;                     ///< Path to gz file
		int32_t     configNum;                      ///< Number of entries
		uint8_t*    configType;                     ///< Entry type
		label_t*    configLabel;                    ///< Entry label
		txtVal_t*   configValue;                    ///< Entry value
	} config_t;

	/// @brief Convert from int to string
	void intToStr(char* result, size_t resultSize, int32_t value);

	/// @brief Convert an integer value to string
	/// @param result The buffer to store the result string
	/// @param resultSize The size of the result buffer
	/// @param value The integer value to convert
	void intToStr(char* result, size_t resultSize, int32_t value);

	/// @brief Initialize configuration from a GZipped file
	/// @param confPtr Pointer to a config_t* that will store the created config
	/// @param gzPath Path to the gzipped config file
	/// @return Status of the operation (confStatus_t)
	confStatus_t initialConfigFromGZFile(config_t ** confPtr, const path_t gzPath);

	/// @brief Create a configuration object
	/// @param confPtr Pointer to a config_t* that will store the created config
	/// @param path Path to the config file
	/// @param confNum Number of configuration entries
	/// @return Status of the operation (confStatus_t)
	confStatus_t createConfig(config_t ** confPtr, const path_t path, int32_t confNum);

	/// @brief Delete a configuration object and free all memory
	/// @param confPtr Pointer to the config_t* to delete
	void deleteConfig(config_t ** confPtr);

	...

```

## 2.4 - Tổ chức mã nguồn

Thông thường, mình sẽ chia các tính năng thành các module và sau đó kết hợp lại. Sơ đồ bên dưới là một ví dụ về cách tổ chức mã nguồn.

```
	.
	├── Makefile
	├── clipboard
	│   ├── clipboard.c
	│   ├── clipboard.h
	│   └── clipboard.o
	├── config
	│   ├── config.c
	│   ├── config.h
	│   └── config.o
	├── helper
	│   └── helper.h
	├── log
	│   ├── log.c
	│   ├── log.h
	│   └── log.o
	├── clipboardService
	├── clipboardService.c
	├── clipboardService.h
	├── clipboardService.o
	├── note.md
	└── test_config.gz

```

Đối vơi các thiết lập toàn cục (global config), các tiện tích (helpers/utils), nên được tổ chức riêng thành các module và sẽ được nhập (include) lại ở mỗi mô đun con.

Sơ đồ bên dưới mô thả một quá trình biên dịch các file.c từ log.c, config.c, clipboard.c và clipboardService.c.

```
	^ Thứ tự truy vết các header (file.h)
	global.h
	^
	├────────────────── log.h
						^
						├────────────────── log.c
	global.h
	^
	├────────────────── log.h
						^
						├────────────────── config.h
											^
											├────────────────────── config.c
	global.h
	^
	├────────────────── log.h
						^
						├────────────────── clipboard.h
											^
											├────────────────────── clipboard.c
	global.h
	^
	├────────────────── log.h
						^
						├────────────────── config.h
											^
											├────────────────────── clipboardService.h
																	^
																	├───────── clipboardService.c
```

Sơ đồ bên dưới mô thả mối quan hệ giữa các module.

```

	───> Thứ tự nhập các mô đun
	
	global.h			log.h(+log.c)			helper.h
	│					│
	├──────────────────>├──────────────────────>├─────────────────────────────────> clipboard.h(+clipboard.c)
												│									│
												│									│
												├───── config.h(+config.c)			│
														│							│
														├──────────────────────────>├─────────> clipboardService.h
																								│
																								├─────────> clipboardService.c
``` 

**NOTE**:
- Do có các định danh macro (VD: `__CONFIG_H__`) kết hợp với các chỉ thị (directive) (VD: `#ifdef` `#ifndef` `#endif`) nên các header file (file.h) sẽ không bị trùng lặp. Các file.c sẽ được biên dịch nhau và liên kết lại bởi linker.
- Các biến toàn cục trong các file này nên dùng `extern` và khai báo chính thức ở file.c nào đó (tốt nhất là trong file.c của mô đun).

### 2.4.1 - Quy ước chung

- Bắt buộc dùng `include guard` cho các file.h.
- Khai báo ở header (file.h) với extern; Định nghĩa duy nhất ở file.c của mô đun chứa biến đó.
- Nguyên tắc DAG (Directed Acyclic Graph): không để hai mô đun phụ thuộc vòng tròn (A → B → A).
- Nguyên tắc layering: mô đun thấp (helper.h) không được include ngược lại mô đun cao hơn (clipboardService.h).

## 2.5 – Quy tắc viết mã (Code Style)

Quy tắc trình bày mã sẽ làm cho cấu trúc mã thống nhất hơn.

- Mỗi hàm một chức năng, không nên gom quá nhiều chức năng vào một hàm.
- Quy ước 01 tab = 04 space
- Mở khối lệnh lại dòng khai báo, dòng điều kiện, ...

```C

/// OKE /////////////////////////////////////////////////////////////////////////////////
	
	typedef struct config_t {
		path_t      configPath;                     /// Path to gz file
		int32_t     configNum;                      /// Number of entries
		uint8_t*    configType;                     /// Entry type
		label_t*    configLabel;                    /// Entry label
		txtVal_t*   configValue;                    /// Entry value
	} config_t;

	void aFunction0(int aParam0, int* aParam1){
		static int coefficient = 19;
		if(aParam0 > 0){
			aParam0 = aParam0 * 99741 / 53246;
		}else if(aParam0 < 0){
			aParam0 = -1 * aParam0;
		}else{
			aParam0 = coefficient * 92641 / 56746;
		}
		coefficient = ((coefficient*aParam0/17)<<2) +  ((coefficient*aParam0/37)>>2);
		*aParam1 = coefficient;
	}
	
	void aFunction1(int aParam0, int* aParam1){
		static int coefficient = 19;
		if(aParam0 > 0){
			aParam0 = aParam0 * 99741 / 53246;
		} else {
			if(aParam0 < 0){
				aParam0 = -1 * aParam0;
			} else {
				aParam0 = coefficient * 92641 / 56746;
			}
		}
		coefficient = ((coefficient*aParam0/17)<<2) +  ((coefficient*aParam0/37)>>2);
		*aParam1 = coefficient;
	}

/// Still valid in C, but not my style /////////////////////////////////////////////////

	typedef struct config_t 
	{
		path_t      configPath;                     /// Path to gz file
		int32_t     configNum;                      /// Number of entries
		uint8_t*    configType;                     /// Entry type
		label_t*    configLabel;                    /// Entry label
		txtVal_t*   configValue;                    /// Entry value
	} 
	config_t;
	
	/// @brief Make a simple random function
	/// @param aParam0 Core of the simple random function 
	/// @param aParam1 The pointer to where stores the random result
	void aFunction(int aParam0, int* aParam1)
	{
		static int coefficient = 19;
		if(aParam0 > 0)
		{
			aParam0 = aParam0 * 99741 / 53246;
		}
		else 
		if(aParam0 < 0){
			aParam0 = -1 * aParam0;
		}
		else
		{
			aParam0 = coefficient * 92641 / 56746;
		}
		coefficient = ((coefficient*aParam0/17)<<2) +  ((coefficient*aParam0/37)>>2);
		*aParam1 = coefficient;
	}

```

# 3 - Một số tiện tích

Các tiện ích này sẽ làm cho code trở nên dễ đọc hơn, ngắn gọn hơn. Ví dụ bên dưới là helper dùng trong SDL2.

```C
	#ifndef __HELPER_H__
	#define __HELPER_H__

	/// @file helper.h
	/// @brief Utility macros for loops, delays, color manipulation, and bit mask operations.
	/// @details
	/// Includes necessary headers (SDL2, time, pthread, etc.) and defines helper macros 
	/// for iteration, sleeping, color handling, and bitwise operations.

	#ifdef LOG_HEADER_INCLUDE
	#pragma message("INCLUDE: global.h")
	#endif

	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <unistd.h>
	#include <time.h>
	#include <math.h>
	#include <pthread.h>
	#include <limits.h>

	/// @name Loop macros
	/// @{

	/// @brief Loop forward from `a` to `b-1`.
	/// @param type Loop variable type.
	/// @param i Loop variable.
	/// @param a Start value (inclusive).
	/// @param b End value (exclusive).
	#define REPT(type, i, a, b) for(type i = (a); (i) < (b); ++(i))

	/// @brief Loop backward from `a` to `b+1`.
	/// @param type Loop variable type.
	/// @param i Loop variable.
	/// @param a Start value (inclusive).
	/// @param b End value (exclusive).
	#define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))

	/// @brief Loop forward using simSize_t.
	/// @param i Loop variable.
	/// @param a Start value (inclusive).
	/// @param b End value (exclusive).
	#define REP(i, a, b) for(simSize_t i = (a); (i) < (b); ++(i))

	/// @brief Loop backward using simSize_t.
	/// @param i Loop variable.
	/// @param a Start value (inclusive).
	/// @param b End value (exclusive).
	#define REV(i, a, b) for(simSize_t i = (a); (i) > (b); --(i))

	/// @}

	/// @name Delay macros
	/// @{

	/// @brief Convert microseconds to nanoseconds.
	#define __USEC(i) ((i) * 1000ULL)

	/// @brief Convert milliseconds to nanoseconds.
	#define __MSEC(i) ((i) * 1000000ULL)

	/// @brief Convert seconds to nanoseconds.
	#define __SEC(i)  ((i) * 1000000000ULL)

	/// @brief Sleep for a given number of milliseconds.
	#define __sleep_ms(ms) do { \
		if ((ms) < 0) break; \
		struct timespec ts; \
		ts.tv_sec  = (ms) / 1000; \
		ts.tv_nsec = ((ms) % 1000) * 1000000L; \
		while (nanosleep(&ts, &ts) == -1 && errno == EINTR); \
	} while(0)

	/// @brief Sleep for a given number of nanoseconds.
	#define __sleep_ns(ns) do { \
		struct timespec ts; \
		ts.tv_sec  = (time_t)((ns) / 1000000000UL); \
		ts.tv_nsec = (long)((ns) % 1000000000UL); \
		nanosleep(&ts, NULL); \
	} while(0)

	/// @brief Sleep for a given number of microseconds.
	#define __sleep_us(us) do { \
		struct timespec ts; \
		ts.tv_sec  = (us) / 1000000; \
		ts.tv_nsec = ((us) % 1000000) * 1000L; \
		nanosleep(&ts, NULL); \
	} while(0)

	/// @brief Sleep for a given number of seconds.
	#define __sleep(sec) do { \
		struct timespec ts; \
		ts.tv_sec  = sec; \
		ts.tv_nsec = 0; \
		nanosleep(&ts, NULL); \
	} while(0)

	/// @}

	/// @name Color macros
	/// @{

	/// @brief Combine RGB values into RGBA (A=255).
	#define __combiRGB(R, G, B) ((((simColor_t)R)<<24) | (((simColor_t)G)<<16) | (((simColor_t)B)<<8) | ((simColor_t)0xFF))

	/// @brief Combine RGBA values into 32-bit color.
	#define __combiRGBA(R, G, B, A) ((((simColor_t)R)<<24) | (((simColor_t)G)<<16) | (((simColor_t)B)<<8) | ((simColor_t)A))

	/// @brief Extract red channel (0-255).
	#define __getRFromRGBA(RGBA) (((simColor_t)(RGBA) >> 24) & 0xFF)

	/// @brief Extract green channel (0-255).
	#define __getGFromRGBA(RGBA) (((simColor_t)(RGBA) >> 16) & 0xFF)

	/// @brief Extract blue channel (0-255).
	#define __getBFromRGBA(RGBA) (((simColor_t)(RGBA) >>  8) & 0xFF)

	/// @brief Extract alpha channel (0-255).
	#define __getAFromRGBA(RGBA) (((simColor_t)(RGBA) >>  0) & 0xFF)

	/// @}

	/// @name Math helpers
	/// @{

	#define __max(a, b) ((a) > (b) ? (a) : (b))  ///< Return max(a,b).
	#define __min(a, b) ((a) < (b) ? (a) : (b))  ///< Return min(a,b).
	#define __abs(x)    ((x) >= 0 ? (x) : -(x))  ///< Return absolute value.

	/// @}

	/// @name Pointer helpers
	/// @{

	#define __is_null(ptr)      ((ptr) == NULL)  ///< Check pointer == NULL.
	#define __is_not_null(ptr)  ((ptr) != NULL)  ///< Check pointer != NULL.

	/// @}

	/// @name Bit mask helpers
	/// @{

	#define __mask8(i)   ((uint8_t)(1U << (i)))     ///< Mask 1 bit in uint8_t.
	#define __mask16(i)  ((uint16_t)(1U << (i)))    ///< Mask 1 bit in uint16_t.
	#define __mask32(i)  ((uint32_t)(1UL << (i)))   ///< Mask 1 bit in uint32_t.
	#define __mask64(i)  ((uint64_t)(1ULL << (i)))  ///< Mask 1 bit in uint64_t.

	#define __inv_mask8(i)   ((uint8_t)(~(1U << (i))))     ///< Inverted mask uint8_t.
	#define __inv_mask16(i)  ((uint16_t)(~(1U << (i))))    ///< Inverted mask uint16_t.
	#define __inv_mask32(i)  ((uint32_t)(~(1UL << (i))))   ///< Inverted mask uint32_t.
	#define __inv_mask64(i)  ((uint64_t)(~(1ULL << (i))))  ///< Inverted mask uint64_t.

	/// @}

	/// @name Value checkers
	/// @{

	#define __is_zero(x)         ((x) == 0)   ///< Check x == 0.
	#define __is_positive(x)     ((x) > 0)    ///< Check x > 0.
	#define __is_negative(x)     ((x) < 0)    ///< Check x < 0.
	#define __isnot_zero(x)      ((x) != 0)   ///< Check x != 0.
	#define __isnot_positive(x)  ((x) <= 0)   ///< Check x <= 0.
	#define __isnot_negative(x)  ((x) >= 0)   ///< Check x >= 0.

	/// @}

	#endif // __HELPER_H__

```











