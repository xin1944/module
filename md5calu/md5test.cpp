#include "md5.h"
#include <iostream>
 
using namespace std;
 
void PrintMD5(const string& str, MD5& md5) {
    cout << "MD5(" << str << ") = " << md5.toString() << endl;
}
 
int main() {
    MD5 md5;
    md5.update("");
    PrintMD5("", md5);

    md5.update("a");
    PrintMD5("a", md5);

    md5.update("abc");
    PrintMD5("abc", md5);

    md5.update("abcdefghijklmnopqrstuvwxyz");
    PrintMD5("abcdefghijklmnopqrstuvwxyz", md5);

    md5.reset();
    md5.update("message digest");
    PrintMD5("message digest", md5);

    md5.reset();
    ifstream in("./test.txt");
    md5.update(in);
    PrintMD5("./test.txt", md5);

    return 0;
}

#if 0

    string strFileName = pFileDis->file_name;
    string strFilePath = strFileName;//pFileDis->tmp_save_path;
    // string strReqFile = pFileDis->file_name;
    // string::size_type pos = strReqFile.rfind('/');
    // string strPath = strReqFile.substr(0, pos + 1);
    // string strFileName = strReqFile.substr(pos + 1);
    // memset(para.file_name, 0, GENERAL_FILE_NAME_LENGTH + 1);
    // memcpy(para.file_name, strFileName.c_str(), strFileName.length());
    // memset(para.path, 0, PATH_LENGTH + 1);
    // memcpy(para.path, strPath.c_str(), strPath.length());
    if (strFileName.find(".cime") || strFileName.find(".CIME"))
    {
        if (globalfunc::fileCheckUpdate(strFilePath + strFileName, "465459899518681613"))
        {
            info_log(SDDL_MODULE_NAME, "cime file has same md5");
            return;
        }
        StFrscsModelRequestMsg* msg = new StFrscsModelRequestMsg;
        msg->msgHead.strID = globalfunc::create_uuid();
        msg->msgHead.command = MODEL_INTERACTION::MI_CIMEMODEL;
        msg->msgHead.type = MT_INQ;
        ostringstream oss;
        oss.str("");
        oss << CEnvironment::get_env()->get_global_config()->strIp << ":" << CEnvironment::get_env()->get_global_config()->iFileServicePort;
        msg->msgHead.strSource = oss.str();
        msg->msgHead.strDest = "";
        msg->msgHead.strInvokerID = "";
        msg->msgHead.iPriority = 0;
        msg->msgHead.iSeqNo = 0;
        msg->msgHead.bContFlag = false;
        msg->msgHead.iPackageTotalNum = 1;
        msg->modelReqArgs.lChannelID = 465459899518681613;
        msg->modelReqArgs.lIedID = 0;
        msg->modelReqArgs.strCimeType = "InterComm";
        msg->modelReqArgs.strCimePath = strFilePath + strFileName;

        //todo
        CEnvironment::get_env()->get_send_thrd()->add_a_message(msg);
    }
    else
    {
        string strErr = "unrecognized file type:";
        strErr += strFileName;
        error_log(SDDL_MODULE_NAME, strErr.c_str());
    }
#endif