#include "main.h"

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void Init(void);		// 初期化
void Uninit(void);		// 終了処理
void Resampling(void);	// 再サンプリング
void OutputFile(void);	// ファイルの書き出し

//=============================================================================
// グローバル変数
//=============================================================================
LOAD_WAV* wavfile = new LOAD_WAV;
WAV_FILE wav;

//=============================================================================
// メイン処理
//=============================================================================
int main(void)
{
	// 初期化
	Init();

	// 再サンプリング
	Resampling();

	// 終了処理
	Uninit();

	return 0;
}

//=============================================================================
// 初期化
//=============================================================================
void Init(void)
{
	// サウンド読み込み
	wavfile->LoadWavFile(SOUND_PATH);

	wav = wavfile->GetWavFile();
}

//=============================================================================
// 再サンプリング
//=============================================================================
void Resampling(void)
{
	// 変換前のサンプリング
	float oldSample = (float)wav.fmt.fmtSampleRate;

	// 音の長さ = 波形のサイズ / 一秒あたりの周波数 / チャネル数 / short型のサイズ
	float soundLengh = ((float)wav.data.waveSize / wav.fmt.fmtSampleRate / wav.fmt.fmtChannel / sizeof(short));

	// ヘッダー情報
	wav.fmt.fmtSampleRate		= 40000;
	wav.fmt.fmtAvgBytesPerSec	= wav.fmt.fmtSampleRate*wav.fmt.fmtBlockAlign;
	wav.data.waveSize			= (long)((wav.fmt.fmtSampleRate*(wav.fmt.fmtBitPerSample / 8)) * soundLengh);

	// バッファ
	short *newBuf = (short *)malloc(sizeof(short) * wav.data.waveSize);

	for (int i = 0; i < wav.data.waveSize; i++)
	{
		int readPos = (int)((float)i * (oldSample / wav.fmt.fmtSampleRate));

		newBuf[i] = wav.data.waveData[readPos];
	}

	wav.data.waveData = newBuf;

	// ファイルの書き出し
	OutputFile();
}

//=============================================================================
// ファイルの書き出し
//=============================================================================
void OutputFile(void)
{
	FILE *fp;
	fp = fopen(OUTPUT_PATH, "wb");

	fwrite(&wav.riff, sizeof(RIFF_CHUNK), 1, fp);
	fwrite(&wav.fmt, sizeof(FMT_CHUNK), 1, fp);
	fwrite(&wav.data, sizeof(DATA_CHUNK) - sizeof(short), 1, fp);

	for (int i = 0; i < wav.data.waveSize; i++)
	{
		fwrite(&wav.data.waveData[i], sizeof(short), 1, fp);
	}

	fclose(fp);
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// サウンド読み込みの終了処理
	delete wavfile;

	if (wav.data.waveData != NULL)
	{
		free(wav.data.waveData);
		wav.data.waveData = NULL;
	}
}