#include "ErrorDef.h"
#include "Ppm.h"

Error_t CPpm::createInstance(CPpm *&pCPpm)
{
    pCPpm = new CPpm ();

    if (!pCPpm)
        return kUnknownError;

    return kNoError;
}

Error_t CPpm::destroyInstance(CPpm *&pCPpm)
{
    if (!pCPpm)
        return kUnknownError;

    delete pCPpm;
    pCPpm = 0;

    return kNoError;
}

CPpm::CPpm()
{
	m_AlphaAT = 0.0f;
	m_AlphaRT = 0.0f;
	m_fSampleRate = 0.0f;
	m_iNumChannels = 0;
	m_epsilon = 1.0f * exp(-5.0f);

}

CPpm::~CPpm()
{
	delete[] m_tempBuff;
	delete[] m_vppmMax;
	m_tempBuff = 0;
	m_vppmMax = 0;
}

Error_t CPpm::initInstance(float fSampleRateInHz, int iNumChannels)
{
	m_fSampleRate = fSampleRateInHz;
	m_iNumChannels = iNumChannels;

	//Set Alpha 
	m_AlphaAT = 1 - exp(-2.2f / (m_fSampleRate*0.01));
	m_AlphaRT = 1 - exp(-2.2f / (m_fSampleRate*1.5));

	//Initialize 
	m_vppmMax = new float[m_iNumChannels];
	for (int i = 0; i < m_iNumChannels; i++)
	{
		m_vppmMax[i] = 0;
	}

	//Initialize
	m_tempBuff = new float[m_iNumChannels];
	for (int i = 0; i < m_iNumChannels; i++)
	{
		m_tempBuff[i] = 0;
	}



}

Error_t CPpm::process(float **ppfInputBuffer, float *ppfOutputBuff, int iNumberOfFrames)
{

	for (int i = 0; i < m_iNumChannels; i++)
	{
		for (int j = 0; j < iNumberOfFrames; j++) {
			if (m_tempBuff[i] > abs(ppfInputBuffer[i][j]))
			{
				ppfOutputBuff[i] = (1 - m_AlphaRT)*m_tempBuff[i];
			}
			else 
			{
				ppfOutputBuff[i] = m_AlphaAT * abs(ppfInputBuffer[i][j]) + (1 - m_AlphaAT)*m_tempBuff[i];
			}
		}
		m_tempBuff[i] = ppfOutputBuff[i];
	}
    return kNoError;
}


 
//function[vppm, t] = FeatureTimePeakEnvelope(x, iBlockLength, iHopLength, f_s)
//
//% number of results
//iNumOfBlocks = ceil(length(x) / iHopLength);
//
//% compute time stamps
//t = ((0:iNumOfBlocks - 1) * iHopLength + (iBlockLength / 2)) / f_s;
//
//% allocate memory
//vppm = zeros(2, iNumOfBlocks);
//v_tmp = zeros(1, iBlockLength);
//
//%initialization
//alpha = 1 - [exp(-2.2 / (f_s * 0.01)), exp(-2.2 / (f_s * 1.5))];
//
//for (n = 1 : iNumOfBlocks)
//i_start = (n - 1)*iHopLength + 1;
//i_stop = min(length(x), i_start + iBlockLength - 1);
//
//% calculate the maximum
//vppm(1, n) = max(abs(x(i_start:i_stop)));
//
//% calculate the PPM value - take into account block overlaps
//% and discard concerns wrt efficiency
//v_tmp = ppm(x(i_start:i_stop), v_tmp(iHopLength), alpha);
//vppm(2, n) = max(v_tmp);
//end
//
//% convert to dB
//epsilon = 1e-5; %-100dB
//
//i_eps = find(vppm < epsilon);
//vppm(i_eps) = epsilon;
//vppm = 20 * log10(vppm);
//end
//
//function[ppmout] = ppm(x, filterbuf, alpha)
//
//% initialization
//alpha_AT = alpha(1);
//alpha_RT = alpha(2);
//
//x = abs(x);
//for (i = 1 : length(x))
//if (filterbuf > x(i))
//% release state
//ppmout(i) = (1 - alpha_RT) * filterbuf;
//else
//% attack state
//ppmout(i) = alpha_AT * x(i) + (1 - alpha_AT) * filterbuf;
//end
//filterbuf = ppmout(i);
//end
//end