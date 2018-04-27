// QPSK.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "QPSK.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CQPSKApp

BEGIN_MESSAGE_MAP(CQPSKApp, CWinApp)
END_MESSAGE_MAP()


// CQPSKApp ����

CQPSKApp::CQPSKApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CQPSKApp ����

CQPSKApp theApp;


// CQPSKApp ��ʼ��

BOOL CQPSKApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//����һ���㷨���󣬲������㷨����ָ��
void *LtCreateObject()
{
	CAlgorithm *pAlgorithm = new CAlgorithm();
	//UserGui->Create(IDD_DIALOG1,NULL);
	return static_cast<void*>( pAlgorithm );
}

//ɾ��һ���㷨���󣬴˺����ڴ��Ĳ�����pObject������LtCreateObject()�ķ���ֵ
void LtDestroyObject( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	//pAlgorithm->DestroyWindow();
	delete pAlgorithm;	//ɾ��һ���㷨����
}

void LtDLLMain(	void * pObject, const bool *pbIsPortUsing, const double *pdInput, double *pdOutput, const int nSimuStep )
{
	ASSERT( pObject != NULL );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->RunAlgorithm( pdInput, pdOutput );
}

void LtResetModule( void *pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->Reset();
}

CAlgorithm::CAlgorithm()
{
	Reset();
}

CAlgorithm::~CAlgorithm()
{
}

void CAlgorithm::Reset()
{
	m_Clk_256k = 0;
	m_Clk_256k_Mid = 0;
	m_Clk_16k = 0;
	m_Clk_16k_mid = 0;
	m_Clk_8k = 0;
	m_Clk_8k_mid = 0;
	m_FIR1_Counter = 0;
	m_FIR1_Result = 0;
	memset( m_FIR1_Buffer, 0, sizeof( m_FIR1_Buffer ) );
	m_FIR1_Num = 0;
	m_FIR2_Counter = 0;
	m_FIR2_Result = 0;
	memset( m_FIR2_Buffer, 0, sizeof( m_FIR2_Buffer ) );
	m_FIR2_Num = 0;

	m_n10MCounter = 0;
	m_10MSin = 0;
	m_10MCos = 0;
	m_MainClk = 0;
	m_nTableIndex = 0;
	m_Cos10M = 0;
	m_Sin10M = 0;

	m_AddResult1 = 0;
	m_AddResult2 = 0;
	m_MultiResult_I_A = 0;
	m_MultiResult_I_B = 0;
	m_MultiResult_Q_A = 0;
	m_MultiResult_Q_B = 0;
	m_MultiResult = 0;
	ResetCostas();
}

void CAlgorithm::ResetCostas()
{
	m_Costas_vco_out = 0;
	m_Costas_sum_ut = 0;
	m_Costas_v8 = 0;
	m_Costas_Counter = 0;
	m_Costas_t = 0;
	m_Costas_vco_out_mid = 0;

	m_LoopFIR_Counter = 0;
	m_LoopFIR_Result = 0;
	memset( m_LoopFIR_Buffer, 0, sizeof( m_LoopFIR_Buffer ) );
	m_LoopFIR_Num = 0;
}

void CAlgorithm::RunAlgorithm(const double *pdInput, double *pdOutput)
{
	MakeModulationCarrier();	//���������ز�
	//����ʱ�Ӷ���Ƶ,QPSK���ƺ����ݷ�ΪI��Q��·�����ʼ���
	if( pdInput[IN2] > 1.0)//64Kʱ��
	{
		if( m_bClkState == LOW )
		{
			m_bClkState = HIGH;
			m_HalfClk++;
			m_HalfClk %= 2;
		}
	}
	else
		m_bClkState = LOW;

	//���������ݵ�ƽ����Ϊ�����������Ա���в�ֱ任
	if( pdInput[IN1] > 1.0 )
		m_nDataIn = 1;
	else
		m_nDataIn = 0;
	if( pdInput[IN2] < m_LastClkState )
		QPSKmid_1 = m_nDataIn;
	if( pdInput[IN2] > m_LastClkState )
		QPSKmid_2 = QPSKmid_1;            /*��λ�����λ*/
	if( pdInput[IN2] < m_LastClkState )
	{
		QPSKmid_I = QPSKmid_2;
		QPSKmid_Q = m_nDataIn;
	}
	if( m_HalfClk > m_LastHalfClkState )//32kʱ��
	{
		I_QPSK = QPSKmid_I*(-2)+1;         /*QPSK��I·��������ｫ�����ź�0��1��Ϊ-1��1���Ա�������ĵ��ƹ�������256K�ز����*/
		Q_QPSK = QPSKmid_Q*(-2)+1;         /*QPSK��Q·���*/
	}
	if( I_QPSK == 1 )
		NRZ_I_QPSK = 3.3;      
	else
		NRZ_I_QPSK = 0;        
	if( Q_QPSK == 1 )
		NRZ_Q_QPSK = 3.3;
	else
		NRZ_Q_QPSK = 0;        /*QPSK���Ƶ�NRZ���*/
	m_LastClkState = int(pdInput[IN2]);
	m_LastHalfClkState = m_HalfClk;

	
//	pdOutput[OUT1] = NRZ_I_QPSK;	//NRZ-I���
//	pdOutput[OUT2] = NRZ_Q_QPSK;	//NRZ-Q���
//	pdOutput[OUT1] = I_QPSK*3.3;	//����ǰ������ƽ�Ļ����ź����
//	pdOutput[OUT2] = Q_QPSK*3.3;
	//�������
	
	pdOutput[OUT5] = I_QPSK*3.3*m_10MCos;	//I·�������
	pdOutput[OUT6] = Q_QPSK*3.3*m_10MSin;	//Q·�������
	pdOutput[OUT7] = I_QPSK*3.3*m_10MCos + Q_QPSK*3.3*m_10MSin;
	pdOutput[OUT3] = m_10MCos;
	//���
	//������β��ʵ��QPSK�ز�ͬ��
	MakeDeModulationCarrier();		//�����ز�
	LoopFilter( m_MultiResult );	//��·�˲���
	pdOutput[OUT1] = m_Costas_v8;
	pdOutput[OUT2] = m_MultiResult;
	pdOutput[OUT3] = m_Costas_vco_out;
	VCO(pdInput[W1]);
	//��һ��:�����ź���ͬ���ز����
	m_DeModulation_I = pdInput[IN3]*m_Cos10M;
	m_DeModulation_Q = pdInput[IN3]*m_Sin10M;
	pdOutput[OUT10] = m_Cos10M;
	//�ڶ���:��˺��źž�����ͨ�˲�ȥ����Ƶ����
	m_DecisionIn_I = Fir16K_I( m_DeModulation_I );
	m_DecisionIn_Q = Fir16K_Q( m_DeModulation_Q );
        //pdOutput[OUT7]=m_DecisionIn_I;
	//���źŽ��������о�������ë��
	Decision( m_DecisionIn_I, m_DecisionIn_Q );
	//��β������
	m_AddResult1 = m_DecisionIn_I + m_DecisionIn_Q;
	m_AddResult2 = m_DecisionIn_I - m_DecisionIn_Q;
	//������ת��Ϊ����ֵ����������������
	if( m_AddResult1 > 0 )
		m_MultiResult_I_A = 2;
	else
		m_MultiResult_I_A = -2;
	if( m_AddResult2 > 0 )
		m_MultiResult_Q_A = 2;
	else
		m_MultiResult_Q_A = -2;
	if( m_Decision_I > 1 )
		m_MultiResult_I_B = 2;
	else
		m_MultiResult_I_B = -2;
	if( m_Decision_Q > 1 )
		m_MultiResult_Q_B = 2;
	else
		m_MultiResult_Q_B = -2;
	m_MultiResult = (m_MultiResult_I_A*m_MultiResult_I_B)*(m_MultiResult_Q_A*m_MultiResult_Q_B);//�����ź�
	//m_MultiResult�������׻�·�˲����õ��������ѹ�ؾ���Ŀ��Ƶ�ѹ

	ClkGen( m_Decision_I );

	//�źŲ����任�õ����
	if( m_Decision_I > 1.0 )              /*I·����*/
		m_DA_Out2 = 0;
	else
		m_DA_Out2 = 3.3;
	if( m_Decision_Q > 1.0 )               /*Q·����*/
		m_DA_Out3 = 0;
	else
		m_DA_Out3 = 3.3;
	if( m_Clk_16k > m_Clk_16k_mid )
		m_QPSK_Q_DE = m_DA_Out3;
	if( m_Clk_16k < m_Clk_16k_mid )
		m_QPSK_Q_DE1 = m_QPSK_Q_DE;
	if( m_Clk_16k > m_Clk_16k_mid && m_Clk_8k < 1 )
		m_QPSK_Diffout = m_DA_Out2;
	else if( m_Clk_16k > m_Clk_16k_mid && m_Clk_8k > 1 )
		m_QPSK_Diffout = m_QPSK_Q_DE1;

	m_Clk_256k_Mid = m_Clk_256k;
	m_Clk_16k_mid = m_Clk_16k;
	m_Clk_8k_mid = m_Clk_8k;

	m_Dout = m_QPSK_Diffout;
	m_BS_Out = m_Clk_16k;

	pdOutput[OUT8] = m_Clk_16k;
	pdOutput[OUT9] = m_Dout;
}

void CAlgorithm::MakeModulationCarrier()
{
	//����ϵͳ���ǲ���2048K�ز�������10.7M�ز�
	m_n10MCounter++;
	m_n10MCounter %= 64;
	int nTableIndex = m_n10MCounter/4;
	if( m_n10MCounter %2 == 0 )
	{
		if( m_MainClk < 1.0 )
		{
			m_MainClk = 3.0;
			m_10MSin = SinTable[nTableIndex];
			m_10MCos = CosineTable[nTableIndex];
		}
		else
			m_MainClk = 0;
	}
}

//2M�ز�
void CAlgorithm::MakeDeModulationCarrier()
{
	if( m_Costas_vco_out >= 0 && m_Costas_vco_out_mid < 0 )//������
	{
		m_nTableIndex++;
		m_nTableIndex %= 16;
	}
	m_Cos10M = CosineTable[m_nTableIndex]*3.3;
	m_Sin10M = SinTable[m_nTableIndex]*3.3;
	m_Costas_vco_out_mid = m_Costas_vco_out;
}

void CAlgorithm::VCO(const double dInverse)
{
	m_Costas_t += Ot;//ʱ���ۼ�
	m_Costas_vco_out = Ac*cos(2*pi*fc*m_Costas_t+2*pi*kc*m_Costas_sum_ut+in_phas);
	m_Costas_sum_ut += Ot*m_Costas_v8*(dInverse-2.5)*50;
}

//��·�˲���,����Ƶ��2048K
void CAlgorithm::LoopFilter(const double DataIn)
{
	m_LoopFIR_Counter++;
	if( m_LoopFIR_Counter > 63 )
	{
		m_LoopFIR_Result = 0.0;
		m_LoopFIR_Counter = 0;
		m_LoopFIR_Buffer[m_LoopFIR_Num] = DataIn;			//�������������Ž�������
		m_LoopFIR_Num++;
		m_LoopFIR_Num %= 51;

		for(int i=0;i<51;i++)		//����fir�˲�����ֵ
		{
			m_LoopFIR_Result = m_LoopFIR_Result + ( m_LoopFIR_Buffer[(m_LoopFIR_Num+i)%51] )*B_LoopFIR[i];
		}
	}
	m_Costas_v8 = m_LoopFIR_Result;                      //���ؼ����ֵ
}


double CAlgorithm::Fir16K_I(const double DataIn)
{
	m_FIR1_Counter++;
	if( m_FIR1_Counter > 4 )
	{
		m_FIR1_Result = 0.0;
		m_FIR1_Counter = 0;
		m_FIR1_Buffer[m_FIR1_Num] = DataIn;			//�������������Ž�������
		m_FIR1_Num++;
		m_FIR1_Num %= 51;

		for(int i=0;i<51;i++)		//����fir�˲�����ֵ
		{
			m_FIR1_Result = m_FIR1_Result + ( m_FIR1_Buffer[(m_FIR1_Num+i)%51] )*B_16K[i];
		}
	}
	return m_FIR1_Result;                      //���ؼ����ֵ
}

double CAlgorithm::Fir16K_Q(const double DataIn)
{
	m_FIR2_Counter++;
	if( m_FIR2_Counter > 4 )
	{
		m_FIR2_Result = 0.0;
		m_FIR2_Counter = 0;
		m_FIR2_Buffer[m_FIR2_Num] = DataIn;			//�������������Ž�������
		m_FIR2_Num++;
		m_FIR2_Num %= 51;

		for(int i=0;i<51;i++)		//����fir�˲�����ֵ
		{
			m_FIR2_Result = m_FIR2_Result + ( m_FIR2_Buffer[(m_FIR2_Num+i)%51] )*B_16K[i];
		}
	}
	return m_FIR2_Result;                      //���ؼ����ֵ
}

//�����о�+����ë��
void CAlgorithm::Decision(const double DataIn_I, const double DataIn_Q)
{
	//I·��ֵ��ƽ�����о�
	if( DataIn_I > -0.25 )
	{
		I2 = 3.3;
		I2_Opposition = 0;
	}
	else
	{
		I2 = 0;
		I2_Opposition = 3.3;
	}
	if( DataIn_I > 0.25 )
		I3 = 3.3;
	else
		I3 = 0;
	//I·�ź�����ë��
	if( I2_Opposition < 1.0 && I3 > 1.0 )
	{
		m_Decision_I = 3.3;
	}
	if( I3 < 1.0 && I2_Opposition > 1.0 )
	{
		m_Decision_I = 0;
	}

	//Q·��ֵ��ƽ�����о�
	if( DataIn_Q > -0.25 )
	{
		Q2 = 3.3;
		Q2_Opposition = 0;
	}
	else
	{
		Q2 = 0;
		Q2_Opposition = 3.3;
	}
	if( DataIn_Q > 0.25 )
		Q3 = 3.3;
	else
		Q3 = 0;

	if( Q2_Opposition < 1.0 && Q3 > 1.0 )
	{
		m_Decision_Q = 3.3;
	}
	if( Q3 < 1.0 && Q2_Opposition > 1.0 )
	{
		m_Decision_Q = 0;
	}
}
//λʱ�Ӳ�����ͬ��
void CAlgorithm::ClkGen(double DataIn)
{
	m_Counter1++;
	if( m_Counter1 > 63 )
	{
		m_Counter1 = 0;
		m_Counter2++;
		m_Counter2 %= 32;
		if( m_Clk_256k == 0 )
			m_Clk_256k = 5;
		else
			m_Clk_256k = 0;
	}
	if( m_Counter2 < 16 )
	{
		m_Clk_16k = 5;
	}
	else
	{
		m_Clk_16k = 0;
	}
	if( m_Clk_16k > m_Clk_16k_mid )
	{
		if( m_Clk_8k == 0 )
			m_Clk_8k = 5;
		else
			m_Clk_8k = 0;
	}

	if( DataIn > 1.0 )
	{
		if( m_DPLL_DataIn_State == false )
		{
			m_DPLL_DataIn_State = true;
			if( m_pulse %16 == 0 )
				m_bIsPhaseLocked = true;
			else
				m_bIsPhaseLocked = false;
			if( m_bIsPhaseLocked == false )
			{
				//�������ǲ����ǳ�ǰ�����ͺ󣬾��������������м�ֵ����������ȷ��ÿ���ܹ�����
				if( ( m_pulse%16 > 0 ) && ( m_pulse%16 < 8 ) )	//��ǰ
					m_Counter2 = 16;
				else	//�ͺ�
					m_Counter2 = 16;
			}
			m_pulse = 0;
		}
		//16��ʱ�Ӽ����������غ��½��ؾ�����
		if( m_Clk_256k > 1 )
		{
			if( m_Clk_256K_State == false )
			{
				m_Clk_256K_State = true;
				m_pulse++;
			}
		}
		else
		{
			if( m_Clk_256K_State == true )
			{
				m_Clk_256K_State = false;
				m_pulse++;
			}
		}
	}
	else
	{
		if( m_DPLL_DataIn_State == true )
		{
			m_DPLL_DataIn_State = false;
			if( m_pulse == 16 )
				m_bIsPhaseLocked = true;
			else
				m_bIsPhaseLocked = false;
			if( m_bIsPhaseLocked == false )
			{
				if( ( m_pulse%16 > 0 ) && ( m_pulse%16 < 8 ) )	//��ǰ
					m_Counter2 = 16;
				else	//�ͺ�
					m_Counter2 = 16;
			}
			m_pulse = 0;
		}
		//16��ʱ�Ӽ����������غ��½��ؾ�����
		if( m_Clk_256k > 1 )
		{
			if( m_Clk_256K_State == false )
			{
				m_Clk_256K_State = true;
				m_pulse++;
			}
		}
		else
		{
			if( m_Clk_256K_State == true )
			{
				m_Clk_256K_State = false;
				m_pulse++;
			}
		}
	}
	/*****************ʱ�Ӳ���***********************/
}
