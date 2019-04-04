#ifndef _LOAD_SOUND_H_
#define	_LOAD_SOUND_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// チャンクを探すループ用
#define FLAG_CHUNK_FMT		(1)			// fmtのフラグ
#define FLAG_CHUNK_DATA		(1<<1)		// dataのフラグ
#define FLAG_CHUNK_END		(3)			// endポイント
// チャンク
#define CHUNK_FMT			("fmt ")	// fmtのチャンク
#define CHUNK_DATA			("data")	// dataのチャンク
#define CHUNK_SIZE			(4)			// チャンクのバイト数
// 判定用
#define CHUNK_SAME			(0)			// memcpyの戻り値(同じ)

//*****************************************************************************
// 構造体
//*****************************************************************************
#pragma pack(push, 1)
typedef struct	// RIFFチャンク 
{
	char	riffChunk[4];
	long	riffSize;
	char	waveChunk[4];
}RIFF_CHUNK;

typedef struct	// fmt チャンク 
{
	char	fmtChunk[4];
	long	fmtSize;
	short	fmtFormatTag;
	short	fmtChannel;
	long	fmtSampleRate;
	long	fmtAvgBytesPerSec;
	short	fmtBlockAlign;
	short	fmtBitPerSample;
}FMT_CHUNK;

typedef struct	// dataチャンク 
{
	char	dataChunk[4];
	long	waveSize;
	short	*waveData;
}DATA_CHUNK;

typedef struct	// WAVファイル 
{
	RIFF_CHUNK	riff;
	FMT_CHUNK	fmt;
	DATA_CHUNK	data;
}WAV_FILE;
#pragma pack(pop)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// win apiで読み込む
bool OpenSound(HWND hWnd, char fileName[], char soundPath[]);

WAVEFORMATEX LoadWavFile(const char *path);	// WAVファイルの読み込む(WAVEFORMATEX構造体)
void UninitLoadSound(void);					// 終了処理
WAV_FILE *GetWavFile(void);					// WAVファイルの情報を取得
#endif