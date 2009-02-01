#define _AFXDLL
#include <afx.h>
#include <iostream>
#include "jni.h"
#include "jawt_md.h"
#include "org_joy_nlp_HLS.h"
#include "HLPubDef.h"
#include "HLSegFunc.h"

using namespace std;
char* jstringToWindows( JNIEnv  *env, jstring jstr )
{
  int length = (env)->GetStringLength(jstr );
  const jchar* jcstr = (env)->GetStringChars(jstr, 0 );
  char* rtn = new char[ length*2+1 ];
  int size = 0;
  size = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)jcstr, length, rtn,(length*2+1), NULL, NULL );
  if( size <= 0 )
    return NULL;
  (env)->ReleaseStringChars(jstr, jcstr );
  rtn[size] = 0;
  return rtn;
}

jstring WindowsTojstring( JNIEnv* env, const char* str )
{
  jstring rtn = 0;
  int slen = strlen(str);
  unsigned short * buffer = 0;
  if( slen == 0 )
    rtn = (env)->NewStringUTF(str ); 
  else
  {
    int length = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)str, slen, NULL, 0 );
    buffer = new unsigned short[ length*2 + 1 ];
    if( MultiByteToWideChar( CP_ACP, 0, (LPCSTR)str, slen, (LPWSTR)buffer, length ) >0 )
      rtn = (env)->NewString(  (jchar*)buffer, length );
  }
  if( buffer )
  delete  buffer ;
  return rtn;
}

JNIEXPORT jboolean JNICALL Java_org_joy_nlp_HLS_HLSplitInit(JNIEnv * env, jobject, jstring jstr)
{
	char *path=jstringToWindows(env,jstr);

	bool bInitDict = HLSplitInit (path);

	return bInitDict;
}

JNIEXPORT void JNICALL Java_org_joy_nlp_HLS_HLFreeSplit
  (JNIEnv *env , jobject)
{
	HLFreeSplit () ;
}

JNIEXPORT jstring JNICALL Java_org_joy_nlp_HLS_HLS_1ParagraphProcess(JNIEnv * env, jobject, jstring jstr, jint istag)
{
	char *c=jstringToWindows(env,jstr);

		CString strText=c;

		HANDLE hHandle = HLOpenSplit (); //�����ִʾ��

		if(hHandle == INVALID_HANDLE_VALUE)
		{//�����ִʾ��ʧ��
			
			return WindowsTojstring(env,"");
		}
	
			
		if(!istag)
		{
			

			int iExtraCalcFlag = 0; //���Ӽ����־�������и��Ӽ���

			bool bSuccess = HLSplitWord (hHandle, (LPCSTR)strText, iExtraCalcFlag);

			CString strResult ;
			strResult = "" ;

			if(bSuccess)
			{//�ִʳɹ�
				int nResultCnt = HLGetWordCnt (hHandle);//ȡ�÷ִʸ���
				for(int i = 0 ; i < nResultCnt ; i++)
				{//ȡ�÷ִʽ��
					SHLSegWord* pWord ;//��ŷִʽ���е�һ����
					pWord = HLGetWordAt (hHandle , i) ;//ȡ��һ���ִʽ��
					strResult += pWord->s_szWord;
					strResult += " " ; //�Կո�ָ�ִʽ���е�ÿ����
				
				}
			char *result=(char*)(LPCTSTR)strResult;
			HLCloseSplit (hHandle) ;//�رշִʾ��

			return WindowsTojstring(env,result);
				

			}
			else
			{//�ִ�ʧ��
				
				HLCloseSplit (hHandle) ;//�رշִʾ��
				return WindowsTojstring(env,"");
			}


		}
		else
		{
			int iExtraCalcFlag = HL_CAL_OPT_POS; //���Ӽ����־
			LPCSTR lpText = (LPCSTR)strText ;
			bool bSuccess = HLSplitWord(hHandle, lpText, iExtraCalcFlag);

			CString strResult ;
			strResult = "";
			if(bSuccess)
			{//�ִʳɹ�
				int nResultCnt = HLGetWordCnt (hHandle);//ȡ�÷ִʸ���
				for(int i = 0 ; i < nResultCnt ; i++)
				{//ȡ�÷ִʽ��
					SHLSegWord* pWord ;//��ŷִʽ���е�һ����
					pWord = HLGetWordAt (hHandle , i) ;//ȡ��һ���ִʽ��
					CString strWord(pWord-> s_szWord) ;


					//���Ա�ע
					
					if((pWord-> s_dwPOS & NATURE_D_A) == NATURE_D_A)
						strWord += "/A";
					if((pWord-> s_dwPOS & NATURE_D_B) == NATURE_D_B)
						strWord += "/B";
					if((pWord-> s_dwPOS & NATURE_D_C) == NATURE_D_C)
						strWord += "/C";
					if((pWord-> s_dwPOS & NATURE_D_D) == NATURE_D_D)
						strWord += "/D";
					if((pWord-> s_dwPOS & NATURE_D_E) == NATURE_D_E)
						strWord += "/E";
					if((pWord-> s_dwPOS & NATURE_D_F) == NATURE_D_F)
						strWord += "/F";
					if((pWord-> s_dwPOS & NATURE_D_H) == NATURE_D_H)
						strWord += "/H";
					if((pWord-> s_dwPOS & NATURE_D_I) == NATURE_D_I)
						strWord += "/I";
					if((pWord-> s_dwPOS & NATURE_D_K) == NATURE_D_K)
						strWord += "/K";
					if((pWord-> s_dwPOS & NATURE_A_M) == NATURE_A_M)
						strWord += "/M";
					if((pWord-> s_dwPOS & NATURE_D_MQ) == NATURE_D_MQ)
						strWord += "/MQ";
					if((pWord-> s_dwPOS & NATURE_D_N) == NATURE_D_N)
						strWord += "/N";
					if((pWord-> s_dwPOS & NATURE_D_O) == NATURE_D_O)
						strWord += "/O";
					if((pWord-> s_dwPOS & NATURE_A_Q) == NATURE_A_Q)
						strWord += "/Q";
					if((pWord-> s_dwPOS & NATURE_D_R) == NATURE_D_R)
						strWord += "/R";
					if((pWord-> s_dwPOS & NATURE_D_S) == NATURE_D_S)
						strWord += "/S";
					if((pWord-> s_dwPOS & NATURE_D_T) == NATURE_D_T)
						strWord += "/T";
					if((pWord-> s_dwPOS & NATURE_D_U) == NATURE_D_U)
						strWord += "/U";
					if((pWord-> s_dwPOS & NATURE_D_V) == NATURE_D_V)
						strWord += "/V";
					if((pWord-> s_dwPOS & NATURE_D_W) == NATURE_D_W)
						strWord += "/W";
					if((pWord-> s_dwPOS & NATURE_D_X) == NATURE_D_X)
						strWord += "/X";
					if((pWord-> s_dwPOS & NATURE_D_Y) == NATURE_D_Y)
						strWord += "/Y";
					if((pWord-> s_dwPOS & NATURE_D_Z) == NATURE_D_Z)
						strWord += "/Z";
					if((pWord-> s_dwPOS & NATURE_A_NR) == NATURE_A_NR)
						strWord += "/NR";
					if((pWord-> s_dwPOS & NATURE_A_NS) == NATURE_A_NS)
						strWord += "/NS";
					if((pWord-> s_dwPOS & NATURE_A_NT) == NATURE_A_NT)
						strWord += "/NT";
					if((pWord-> s_dwPOS & NATURE_A_NX) == NATURE_A_NX)
						strWord += "/NX";
					if((pWord-> s_dwPOS & NATURE_A_NZ) == NATURE_A_NZ)
						strWord += "/NZ";



					

					
					strResult +=strWord ;
					strResult +=" " ; //�Կո�ָ�ִʽ���е�ÿ����
				}
				char *result=(char*)(LPCTSTR)strResult;
				HLCloseSplit (hHandle) ;//�رշִʾ��

				return WindowsTojstring(env,result);
			}	
			else
			{//�ִ�ʧ��
				
				HLCloseSplit (hHandle) ;//�رշִʾ��
				return WindowsTojstring(env,"");
			}

	}
}