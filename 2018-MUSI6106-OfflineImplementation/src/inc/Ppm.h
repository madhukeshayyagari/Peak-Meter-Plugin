#if !defined(__Ppm_hdr__)
#define __Ppm_hdr__

#include "ErrorDef.h"
#include "math.h"

class CPpm
{
public:
    static Error_t createInstance (CPpm*& pCPpm);
    static Error_t destroyInstance (CPpm*& pCPpm);
	Error_t CPpm::initInstance(float fSampleRateInHz, int iNumChannels);
    Error_t process (float **ppfInputBuffer, float *ppfOutputBuff, int iNumberOfFrames);
	Error_t reset();
protected:
    CPpm ();
    virtual ~CPpm ();

private:
	float m_fSampleRate;
	int m_iNumChannels;
	float m_AlphaAT;
	float m_AlphaRT;
	float m_epsilon;
	float m_currentValue;

	float *m_tempBuff;
	float *m_vppmMax;

};

#endif // #if !defined(__Ppm_hdr__)