
#pragma once

//! 処理失敗を表すビットマスク
static const u32 FAIL_MASK = (1 << 31);

//! リターンコードの定義
enum ReturnCode {
	R_SUCCESS_BASE = 0x00000000,
	R_SUCCESS, //!< 正常終了

	R_FAIL_BASE = FAIL_MASK,

	R_FAIL, //!< 異常終了
	R_FAIL_MULTIPLE_INITIALIZE,	//!< 多重初期化
	R_FAIL_MULTIPLE_FINALIZE,		//!< 多重解放
	R_FAIL_NOT_IMPLEMENTED,		//!< 未実装
	R_FAIL_OPEN_FILE,					//!< ファイル開けなかった
};

//! リターンコードの構造体
struct Retval {
	u32 code; //!< 生リターンコード

	//! R_SUCCESS_**ならtrue
	bool isSuccess() const {
		return (code & FAIL_MASK) == 0;
	}

	//! R_FAIL_**ならtrue
	bool isFail() const {
		return !isSuccess();
	}

	Retval(ReturnCode iCode)
	: code(iCode)
	{ }

}; // struct Retval
