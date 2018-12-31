#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::string;

typedef struct  WAV_HEADER
{
    /* RIFF Chunk Descriptor */
    uint8_t         RIFF[4];        // RIFF Header Magic header
    uint32_t        ChunkSize;      // RIFF Chunk Size
    uint8_t         WAVE[4];        // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t         fmt[4];         // FMT header
    uint32_t        Subchunk1Size;  // Size of the fmt chunk
    uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t        bytesPerSec;    // bytes per second
    uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t         Subchunk2ID[4]; // "data"  string
    uint32_t        Subchunk2Size;  // Sampled data length
} wav_hdr;

// Function prototypes
int getFileSize(FILE* inFile);

int main(int argc, char* argv[])
{
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr), filelength = 0;

    const char* filePath;
    string input;
    if (argc <= 1)
    {
        cout << "Input wave file name: ";
        cin >> input;
        cin.get();
        filePath = input.c_str();
    }
    else
    {
        filePath = argv[1];
        cout << "Input wave file name: " << filePath << endl;
    }

    FILE* wavFile = fopen(filePath, "r");
    if (wavFile == nullptr)
    {
        fprintf(stderr, "Unable to open wave file: %s\n", filePath);
        return 1;
    }

    /*********************************************************
     It reads the wavFile up to 1*headersize and put the result
     inside the memory structure defined by wavHeader struct.
     This way we can access to all metadata information in the wav file
     and also move the reading pointer to the start of PCM data
    ************************************************************/
    //Read the header
    size_t bytesRead = fread(&wavHeader, 1, headerSize, wavFile);
    cout << "Header Read " << bytesRead << " bytes." << endl;
    if (bytesRead > 0)
    {
        //Read the data
        uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      //Number     of bytes per sample
        uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; //How many samples are in the wav file?
        static const uint16_t BUFFER_SIZE = 4096;
        //int8_t* buffer_byte = new int8_t[BUFFER_SIZE];
        int16_t* buffer_word = new int16_t[BUFFER_SIZE];
        int16_t* buffer_pcm_out = new int16_t[BUFFER_SIZE*8];
        size_t wordsRead;
        int32_t buff_cnt;
        buff_cnt = 0;
        // while ((bytesRead = fread(buffer_byte, sizeof buffer_byte[0], BUFFER_SIZE / (sizeof buffer_byte[0]), wavFile)) > 0)
        // {
        //     /** DO SOMETHING WITH THE WAVE DATA HERE **/
        //     cout << "Read " << bytesRead << " bytes." << endl;
        // }
        while((wordsRead = fread(buffer_word, sizeof(buffer_word[0]), BUFFER_SIZE/ (sizeof(buffer_word[0])), wavFile )) > 0)
        {
            
            cout<< "Read " << wordsRead << " 16bits words." << endl;
            cout<<buff_cnt+1<< " time(s) the buffer filled"<<endl;

            for(int i=0; i<BUFFER_SIZE; i++)
            {
            // It seems buffer_word does not capture everythin
            buffer_pcm_out[i+((BUFFER_SIZE*buff_cnt)+1)]=(buffer_word[i]);

            }
            buff_cnt++;

        }

        FILE* outFile;
        outFile = fopen("output.wav","wb");
        fwrite(buffer_pcm_out,sizeof(int16_t),/*sizeof(buffer_pcm_out)*/(BUFFER_SIZE*10),outFile);
        fclose(outFile);

        //delete [] buffer_byte;
        delete [] buffer_word;
        delete [] buffer_pcm_out;
        //buffer_byte = nullptr;
        buffer_word = nullptr;
        buffer_pcm_out = nullptr;
        filelength = getFileSize(wavFile);

        cout << "File is                    :" << filelength << " bytes." << endl;
        cout << "RIFF header                :" << wavHeader.RIFF[0] << wavHeader.RIFF[1] << wavHeader.RIFF[2] << wavHeader.RIFF[3] << endl;
        cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << endl;
        cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << endl;
        cout << "Data size                  :" << wavHeader.ChunkSize << endl;

        // Display the sampling Rate from the header
        cout << "Sampling Rate              :" << wavHeader.SamplesPerSec << endl;
        cout << "Number of bits used        :" << wavHeader.bitsPerSample << endl;
        cout << "Number of channels         :" << wavHeader.NumOfChan << endl;
        cout << "Number of bytes per second :" << wavHeader.bytesPerSec << endl;
        cout << "Data length                :" << wavHeader.Subchunk2Size << endl;
        cout << "Audio Format               :" << wavHeader.AudioFormat << endl;
        // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

        cout << "Block align                :" << wavHeader.blockAlign << endl;
        cout << "Data string                :" << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;
    }
    fclose(wavFile);
    
    return 0;
}

// find the file size
int getFileSize(FILE* inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}