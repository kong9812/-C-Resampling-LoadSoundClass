#include "loadSoundClass.h"

//=============================================================================
// �I������
//=============================================================================
LOAD_WAV::~LOAD_WAV()
{
	if (wavFile.data.waveData != NULL)
	{
		free(wavFile.data.waveData);
		wavFile.data.waveData = NULL;
	}
}

//=============================================================================
// �T�E���h�̓ǂݍ���
//=============================================================================
bool LOAD_WAV::OpenSound(HWND hWnd, char fileName[], char soundPath[])
{
	OPENFILENAME ofn;
	char path[CHAR_MAX];
	WIN32_FIND_DATA fileData;

	// �J�����g�f�B���N�g�������o��
	GetCurrentDirectory(MAX_PATH, oldPath);

	path[0] = '\0';
	ofn.hwndOwner = hWnd;
	memset(&ofn, 0, sizeof(OPENFILENAME));  //�\���̂�0�ŃN���A
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrInitialDir = oldPath;
	ofn.lpstrFilter = "sound file(*.wav)\0*.wav\0\0";
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrDefExt = "wav";

	if (GetOpenFileName(&ofn) != TRUE)
	{
		return FALSE;
	}

	strcpy(soundPath, path);

	// �t�@�C���������o��
	FindFirstFile(ofn.lpstrFile, &fileData);

	// �t�@�C����
	fileName = fileData.cFileName;

	// �J�����g�f�B���N�g����ݒ肷��
	SetCurrentDirectory(oldPath);

	return TRUE;
}

//*****************************************************************************
// WAV�t�@�C���̓ǂݍ���(WAVEFORMATEX�\����)
//*****************************************************************************
WAVEFORMATEX LOAD_WAV::LoadWavFile(const char *path)
{
	WAVEFORMATEX	pcm;				// WAVEFORMATEX�\����
	FILE			*fp;				// �t�@�C���\����							
	int				chunkFlag = NULL;	// �`�����N��T�����[�v�̃t���O

	{// ���[�J���ϐ��̏�����
		memset(&pcm, 0, sizeof(WAVEFORMATEX));
		memset(&fp, 0, sizeof(FILE));
	}
	// �t�@�C���̏���
	fopen_s(&fp, path, "rb");

	// RIFF�̓ǂݍ���
	fread(&wavFile.riff, sizeof(RIFF_CHUNK), 1, fp);

	// FMT��DATA�̓ǂݍ���
	while (chunkFlag != FLAG_CHUNK_END)
	{
		char	chunk[CHUNK_SIZE];
		long	size = 0;

		// �`�����N�ƃT�C�Y�̓ǂݍ���
		fread(&chunk, sizeof(chunk), 1, fp);
		fread(&size, sizeof(size), 1, fp);

		// fmt�`�����N
		if (memcmp(chunk, CHUNK_FMT, CHUNK_SIZE) == CHUNK_SAME)
		{
			// �`�����N�ƃT�C�Y��ݒ�
			memcpy(wavFile.fmt.fmtChunk, chunk, CHUNK_SIZE);
			wavFile.fmt.fmtSize = size;

			// �t�H�[�}�b�gID����ǂݍ���
			fread(&wavFile.fmt.fmtFormatTag,
				sizeof(FMT_CHUNK) - 8, 1, fp);

			// �t���O����
			chunkFlag += FLAG_CHUNK_FMT;

			continue;
		}

		// data�`�����N
		if (memcmp(chunk, CHUNK_DATA, CHUNK_SIZE) == CHUNK_SAME)
		{
			// �`�����N�ƃT�C�Y��ݒ�
			memcpy(wavFile.data.dataChunk, CHUNK_DATA, CHUNK_SIZE);
			wavFile.data.waveSize = size;

			// �������m��
			wavFile.data.waveData = (short *)malloc(wavFile.data.waveSize);

			// �g�`�̓ǂݍ���
			fread(wavFile.data.waveData, wavFile.data.waveSize, 1, fp);

			// �t���O����
			chunkFlag += FLAG_CHUNK_DATA;

			continue;
		}

		// ���̃`�����N
		else
		{
			// size����skip
			fseek(fp, size, SEEK_CUR);
		}
	}

	/* wav�\���̂̏����� */
	pcm.cbSize = 0;
	pcm.nChannels = wavFile.fmt.fmtChannel;
	pcm.wBitsPerSample = wavFile.fmt.fmtBitPerSample;
	pcm.nSamplesPerSec = wavFile.fmt.fmtSampleRate;
	pcm.wFormatTag = WAVE_FORMAT_PCM;
	pcm.nBlockAlign = (wavFile.fmt.fmtChannel*wavFile.fmt.fmtBitPerSample) / 8;
	pcm.nAvgBytesPerSec = wavFile.fmt.fmtSampleRate*pcm.nBlockAlign;

	fclose(fp);

	return pcm;
}

//*****************************************************************************
// WAV�t�@�C���̏����擾
//*****************************************************************************
WAV_FILE LOAD_WAV::GetWavFile(void)
{
	return wavFile;
}
