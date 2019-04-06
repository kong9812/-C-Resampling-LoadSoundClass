#include "main.h"

//=============================================================================
// �v���g�^�C�v�錾
//=============================================================================
void Init(void);				// ������
void Uninit(void);				// �I������
void Resampling(void);			// �ăT���v�����O
void OutputFile(short *wave);	// �t�@�C���̏����o��

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
LOAD_WAV* wavfile = new LOAD_WAV;
WAV_FILE wav;

//=============================================================================
// ���C������
//=============================================================================
int main(void)
{
	// ������
	Init();

	// �ăT���v�����O
	Resampling();

	// �I������
	Uninit();

	return 0;
}

//=============================================================================
// ������
//=============================================================================
void Init(void)
{
	// �T�E���h�ǂݍ���
	wavfile->LoadWavFile(SOUND_PATH);

	wav = wavfile->GetWavFile();
}

//=============================================================================
// �ăT���v�����O
//=============================================================================
void Resampling(void)
{
	// �ϊ��O�̃T���v�����O
	float oldSample = (float)wav.fmt.fmtSampleRate;

	// ���̒��� = �g�`�̃T�C�Y / ��b������̎��g�� / �`���l���� / short�^�̃T�C�Y
	float soundLengh = ((float)wav.data.waveSize / wav.fmt.fmtSampleRate / wav.fmt.fmtChannel / sizeof(short));

	// �w�b�_�[���
	wav.fmt.fmtSampleRate		= 48000;
	wav.fmt.fmtAvgBytesPerSec	= wav.fmt.fmtSampleRate*wav.fmt.fmtBlockAlign;
	wav.data.waveSize = (long)(wav.fmt.fmtSampleRate * wav.fmt.fmtChannel * soundLengh * sizeof(short));

	// �o�b�t�@
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

	// �t�@�C���̏����o��
	OutputFile(newBuf);

	if (newBuf != NULL)
	{
		free(newBuf);
		newBuf = NULL;
	}
}

//=============================================================================
// �t�@�C���̏����o��
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
// �I������
//=============================================================================
void Uninit(void)
{
	// �T�E���h�ǂݍ��݂̏I������
	delete wavfile;
}