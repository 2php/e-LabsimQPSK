// QPSK.h : QPSK DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#define CL 61
#define L_CL 102
#define B_CL 61

const double pi=3.141592653589793;//��=3.1415
const double Ac=1.28;				//����
const double fc=32768000;			//����Ƶ��32767.9K
const double kc=231.9765625;		//������Hz/V
const double in_phas=pi;			//��ʼ��λpi
const double Ot=7.6294e-009;		//VCO�������е�ʱ����Ĭ�������ʱ�ӵ���131072KHz

#define	LOW		false
#define	HIGH	true
const double SinTable[16] = {0, 0.406736643075800, 0.743144825477394, 0.951056516295154, 0.994521895368273,
							0.866025403784439, 0.587785252292473, 0.207911690817759, -0.207911690817759,
							-0.587785252292473, -0.866025403784439, -0.994521895368273, -0.951056516295154,
							-0.743144825477394, -0.406736643075800, -2.44929359829471e-16};

const double CosineTable[16] = {1, 0.913545457642601, 0.669130606358858, 0.309016994374947, -0.104528463267653,
								-0.500000000000000, -0.809016994374947, -0.978147600733806, -0.978147600733806,
								-0.809016994374948, -0.500000000000000, -0.104528463267654, 0.309016994374947, 
								0.669130606358859, 0.913545457642601, 1};
const int BL = 51;
const double B_16K[BL] = {
	0.002800758778771,  0.00294254671171, 0.003339503420667, 0.003989015999808,
   0.004884114808098, 0.006013557358537, 0.007361988364593, 0.008910173589158,
	0.01063530377509,  0.01251136363174,  0.01450955963364,  0.01659879927928,
	0.03093187554288,  0.02915288169858,  0.02722980370754,  0.02519400620257,
	0.01874621348076,  0.02091771292806,  0.02307856861128,  0.02519400620257,
	0.02722980370754,  0.02915288169858,  0.03093187554288,  0.03253767933468,
	0.03394395172867,  0.03512757454171,  0.03606905583002,  0.03675287014403,
	0.03716772979423,  0.03730678220779,  0.03716772979423,  0.03675287014403,
	0.03606905583002,  0.03512757454171,  0.03394395172867,  0.03253767933468,
	0.02307856861128,  0.02091771292806,  0.01874621348076,  0.01659879927928,
	0.01450955963364,  0.01251136363174,  0.01063530377509, 0.008910173589158,
   0.007361988364593, 0.006013557358537, 0.004884114808098, 0.003989015999808,
   0.003339503420667,  0.00294254671171, 0.002800758778771
};

//2K��ͨ���ڻ�·�˲���
const double B_LoopFIR[51] = {
   0.002944521525459, 0.003078975488574,   0.0034785378403, 0.004137139632189,
   0.005044601838735, 0.006186794513853, 0.007545858805151, 0.009100488326631,
    0.01082626544275,  0.01269604713945,  0.01468039436498,  0.01674803802807,
    0.01886637425381,  0.02100198102846,  0.02312114802089,  0.02519041115528,
    0.02717708343139,  0.02904977354621,  0.03077888406243,  0.03233708119364,
    0.03369972872612,   0.0348452791674,  0.03575561589207,  0.03641634083493,
    0.03681700314887,  0.03695126518472,  0.03681700314887,  0.03641634083493,
    0.03575561589207,   0.0348452791674,  0.03369972872612,  0.03233708119364,
    0.03077888406243,  0.02904977354621,  0.02717708343139,  0.02519041115528,
    0.02312114802089,  0.02100198102846,  0.01886637425381,  0.01674803802807,
    0.01468039436498,  0.01269604713945,  0.01082626544275, 0.009100488326631,
   0.007545858805151, 0.006186794513853, 0.005044601838735, 0.004137139632189,
     0.0034785378403, 0.003078975488574, 0.002944521525459
};


// CQPSKApp
// �йش���ʵ�ֵ���Ϣ������� QPSK.cpp
//

class CQPSKApp : public CWinApp
{
public:
	CQPSKApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

class CAlgorithm
{
public:
	CAlgorithm(void);
	virtual ~CAlgorithm(void);
	void	RunAlgorithm(const double *pdInput, double *pdOutput);
	void	Reset();
	enum IN_PortName{IN1=0, IN2, IN3, IN4, IN5, IN6, IN7, IN8, IN9, IN10, W1, W2, W3, W4};
	enum OUT_PortName{OUT1=0, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7, OUT8, OUT9, OUT10};
private:
	bool	m_bClkState;
	unsigned int m_nDataIn;
	double	m_LastClkState;
	int		m_HalfClk;
	int		m_LastHalfClkState;		//����ʱ�Ӷ���Ƶ

	void	MakeModulationCarrier();	//���������ز�
	unsigned int m_n10MCounter;
	double	m_10MSin;
	double	m_10MCos;
	double	m_MainClk;
	void	MakeDeModulationCarrier();	//��������ز�
	int		m_nTableIndex;
	double	m_Cos10M;
	double	m_Sin10M;
	void	ClkGen(double DataIn);
	int QPSKmid_1;
	int QPSKmid_2;
	int QPSKmid_I;
	int QPSKmid_Q;
	double NRZ_I_QPSK;
	double NRZ_Q_QPSK;
	int I_QPSK;
	int Q_QPSK;

	void	Decision(const double DataIn_I, const double DataIn_Q);
	double	Fir16K_I(const double DataIn);
	double	Fir16K_Q(const double DataIn);
	void	ResetCostas();
	void	LoopFilter(const double DataIn);
	void	VCO(const double dInverse);
	double	m_DeModulation_I;
	double	m_DeModulation_Q;
	double	m_DecisionIn_I;
	double	m_DecisionIn_Q;
	double	m_Decision_I;
	double	m_Decision_Q;

	//��ͨ�˲���1���� ����I·
	int		m_FIR1_Counter;
	double	m_FIR1_Result;
	double	m_FIR1_Buffer[51];
	int		m_FIR1_Num;
	//��ͨ�˲���2���� ����Q·
	int		m_FIR2_Counter;
	double	m_FIR2_Result;
	double	m_FIR2_Buffer[51];
	int		m_FIR2_Num;
	//��·�˲�������
	int		m_LoopFIR_Counter;
	double	m_LoopFIR_Result;
	double	m_LoopFIR_Buffer[51];
	int		m_LoopFIR_Num;

	//�����о�������ë��
	double			I2;	//�������о����
	double			I2_Opposition;	//I2��������
	double			I3;	//�������о���� 
	double			Q2;
	double			Q2_Opposition;
	double			Q3;

	//QPSK�����任����
	double			m_QPSK_Q_DE;
	double			m_QPSK_Q_DE1;
	double			m_QPSK_Diffout;
	double			m_QPSK_I_Mid;
	double			m_DA_Out1;
	double			m_DA_Out2;
	double			m_DA_Out3;
	double			m_Dout;
	double			m_BS_Out;

	//λʱ��ͬ��ģ�����
	unsigned int	m_Clk_256k;
	unsigned int	m_Clk_16k;
	unsigned int	m_Clk_256k_Mid;
	unsigned int	m_Counter1;		//256K��Ƶ������
	unsigned int	m_Counter2;		//16K��Ƶ������
	bool			m_DPLL_DataIn_State;	//�������໷����ֵ״̬
	bool			m_Clk_256K_State;		//16��ʱ��״̬
	bool			m_bIsPhaseLocked;		//���໷��λ����״̬
	unsigned int	m_pulse;	//16����ʱ�Ӳ�����������
	bool			m_Clk_16k_State;	//16Kʱ������Ϣ
	unsigned int	m_Clk_16k_mid;
	unsigned int	m_Clk_8k;
	unsigned int	m_Clk_8k_mid;
	double			m_LastValue;	//�����о����ֵ

	//��β��
	double			m_AddResult1;
	double			m_AddResult2;
	int				m_MultiResult_I_A;
	int				m_MultiResult_I_B;
	int				m_MultiResult_Q_A;
	int				m_MultiResult_Q_B;
	int				m_MultiResult;

	//VCO
	unsigned int	m_BandPassCounter;
	double	m_Costas_output;
	//***********variable of filter2********************/
	double	m_Costas_v6;						//filter1����ź�
	double	m_Costas_temp_v4[CL];				//�������ݻ���
	double	m_Costas_temp_v6;					//�������
	//***********variable of VCO************************/
	double	m_Costas_vco_out;					//vco���256K�ź�,��������
	double	m_Costas_t;						//����ʱ��t
	double	m_Costas_sum_ut;					//��¼����ֵ
	//**********variable of loop filter********************/
	double	m_Costas_v8;
	double	m_Costas_vco_out_mid;
	int		m_Costas_Counter;
};