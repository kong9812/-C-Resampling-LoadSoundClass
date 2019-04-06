#include "main.h"

//=============================================================================
// プロトタイプ宣言
//=============================================================================
void Init(void);				// 初期化
void Uninit(void);				// 終了処理
void Resampling(void);			// 再サンプリング
void OutputFile(short *wave);	// ファイルの書き出し

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
	wav.fmt.fmtSampleRate		= 48000;
	wav.fmt.fmtAvgBytesPerSec	= wav.fmt.fmtSampleRate*wav.fmt.fmtBlockAlign;
	wav.data.waveSize = (long)(wav.fmt.fmtSampleRate * wav.fmt.fmtChannel * soundLengh * sizeof(short));

	// バッファ
	short *newBuf = (short *)malloc(wav.data.waveSize);
	memset(newBuf, 0, wav.data.waveSize);

	for (int i = 0; i < (wav.data.waveSize / (int)sizeof(short) / wav.fmt.fmtChannel); i++)
	{
		int		readPos = (int)((float)i * (oldSample / wav.fmt.fmtSampleRate));
		float	tmpPos	= (float)i * (oldSample / wav.fmt.fmtSampleRate);

		tmpPos -= (int)tmpPos;

		for (int j = 0; j < wav.fmt.fmtChannel; j++)
		{
			newBuf[i * wav.fmt.fmtChannel + j] =
				(short)(wav.data.waveData[readPos*wav.fmt.fmtChannel + j] + ((float)(wav.data.waveData[readPos *wav.fmt.fmtChannel + j] - wav.data.waveData[readPos*wav.fmt.fmtChannel + j]) * tmpPos));
		}
	}

	// ファイルの書き出し
	OutputFile(newBuf);

	if (newBuf != NULL)
	{
		free(newBuf);
		newBuf = NULL;
	}
}

//=============================================================================
// ファイルの書き出し
//=============================================================================
void OutputFile(short *wave)
{
	FILE *fp;
	fp = fopen(OUTPUT_PATH, "wb");

	fwrite(&wav.riff, sizeof(RIFF_CHUNK), 1, fp);
	fwrite(&wav.fmt, sizeof(FMT_CHUNK), 1, fp);
	fwrite(&wav.data, sizeof(DATA_CHUNK) - 4, 1, fp);

	fwrite(wave, wav.data.waveSize, 1, fp);

	fclose(fp);
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// サウンド読み込みの終了処理
	delete wavfile;
}