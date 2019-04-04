#include "main.h"

//=============================================================================
// �v���g�^�C�v�錾
//=============================================================================
void Init(void);		// ������
void Uninit(void);		// �I������
void Resampling(void);	// �ăT���v�����O
void OutputFile(void);	// �t�@�C���̏����o��

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
	wav.fmt.fmtSampleRate		= 40000;
	wav.fmt.fmtAvgBytesPerSec	= wav.fmt.fmtSampleRate*wav.fmt.fmtBlockAlign;
	wav.data.waveSize			= (long)((wav.fmt.fmtSampleRate*(wav.fmt.fmtBitPerSample / 8)) * soundLengh);

	// �o�b�t�@
	short *newBuf = (short *)malloc(sizeof(short) * wav.data.waveSize);

	for (int i = 0; i < wav.data.waveSize; i++)
	{
		int readPos = (int)((float)i * (oldSample / wav.fmt.fmtSampleRate));

		newBuf[i] = wav.data.waveData[readPos];
	}

	wav.data.waveData = newBuf;

	// �t�@�C���̏����o��
	OutputFile();
}

//=============================================================================
// �t�@�C���̏����o��
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
// �I������
//=============================================================================
void Uninit(void)
{
	// �T�E���h�ǂݍ��݂̏I������
	delete wavfile;

	if (wav.data.waveData != NULL)
	{
		free(wav.data.waveData);
		wav.data.waveData = NULL;
	}
}