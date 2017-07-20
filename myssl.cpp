#include "myssl.h"
#include "ui_myssl.h"

#include <QDebug>
#include <openssl/ssl.h>
#include <openssl/aes.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#define AES_BITS 128
#define MSG_LEN 128

char key[AES_BLOCK_SIZE];
char sourceStringTemp[MSG_LEN];
char dstStringTemp[MSG_LEN];

MySSl::MySSl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySSl)
{
    ui->setupUi(this);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("openssl.db");
    if(m_db.open())
    {
        qDebug()<<"m_db Opened";

        QSqlQuery sql_query;
        QString create_sql = "create table if not exists info (src varchar(30) primary key, dst varchar(30), key varchar(30))"; //创建数据表
//        QString insert_sql = "insert into info values(?,?,?)";    //插入数据
//        QString select_all_sql = "select * from info";

        sql_query.prepare(create_sql); //创建表
        if(!sql_query.exec()) //查看创建表是否成功
        {
            qDebug()<<QObject::tr("Table Create failed");
            qDebug()<<sql_query.lastError();
        }
        else
        {
            qDebug()<< "Table Created" ;

            //插入数据
//            sql_query.prepare(insert_sql);

//            QVariantList GroupIDs;
//            GroupIDs.append("0");
//            GroupIDs.append("1");
//            GroupIDs.append("2");

//            QVariantList GroupNames;
//            GroupNames.append("hsp");
//            GroupNames.append("rl");
//            GroupNames.append("spl");

//            QVariantList GroupAddress;
//            GroupAddress.append("南充");
//            GroupAddress.append("宝鸡");
//            GroupAddress.append("南充");

//            sql_query.addBindValue(GroupIDs);
//            sql_query.addBindValue(GroupNames);
//            sql_query.addBindValue(GroupAddress);

//            if(!sql_query.execBatch())
//            {
//                qDebug()<<sql_query.lastError();
//            }
//            else
//            {
//                qDebug()<<"插入记录成功";
//            }

//            //查询所有记录
//            sql_query.prepare(select_all_sql);
//            if(!sql_query.exec())
//            {
//                qDebug()<<sql_query.lastError();
//            }
//            else
//            {
//                while(sql_query.next())
//                {
//                    int id = sql_query.value(0).toInt();
//                    QString name = sql_query.value(1).toString();
//                    QString address = sql_query.value(2).toString();
//                    qDebug()<<QString("ID:%1  Name:%2  Address:%3").arg(id).arg(name).arg(address);
//                }
//            }
        }
    }
    m_db.close();
}

MySSl::~MySSl()
{
    delete ui;
}

//加密
std::string MySSl::sha256(const std::string str)
{
    char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string newString = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        newString = newString + buf;
    }
    return newString;
}

//加密
int MySSl::aes_encrypt(char* in, char* key, char* out)//, int olen)可能会设置buf长度
{
    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
    for(int i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
        iv[i]=0;
    AES_KEY aes;
    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
    int len=strlen(in);//这里的长度是char*in的长度，但是如果in中间包含'\0'字符的话

    //那么就只会加密前面'\0'前面的一段，所以，这个len可以作为参数传进来，记录in的长度

    //至于解密也是一个道理，光以'\0'来判断字符串长度，确有不妥，后面都是一个道理。
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_ENCRYPT);

    return 1;
}

//解密
int MySSl::aes_decrypt(char* in, char* key, char* out)
{
    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
    for(int i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
        iv[i]=0;
    AES_KEY aes;
    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
    int len=strlen(in);
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_DECRYPT);
    return 1;
}

void MySSl::openDB()
{
    if(m_db.open()){
        qDebug() << "m_db open success!";
    }else{
        qDebug() << "m_db open fail!";
    }
}

void MySSl::closeDB()
{
    m_db.close();
}

void MySSl::on_btn_encry_clicked()
{
#if 0
    QString tmp = ui->textEdit->toPlainText();
    std::string str = tmp.toStdString();
    qDebug() << "str:" << tmp;
    std::string encryStr = sha256(str);
    qDebug() << QString::fromStdString(encryStr);
    ui->textEdit_2->setText(QString::fromStdString(encryStr));
#else
    QString tmp,str;
    memset((char*)sourceStringTemp, 0 ,MSG_LEN);
    memset((char*)dstStringTemp, 0 ,MSG_LEN);
    QString text = ui->textEdit->toPlainText();
    strcpy((char*)sourceStringTemp, text.toLatin1().data());

//    for(int i = 0; i < 16; ++i)//可自由设置密钥
//    {
//        key[i] = 32 + i;
//    }

    QString iKey = ui->lineEdit->text();
    if(iKey.isEmpty()){
        QMessageBox::warning(this,tr("警告："),tr("密钥为空！"));
        return;
    }
    strcpy((char*)key, iKey.toLatin1().data());

    //加密
    if(!aes_encrypt(sourceStringTemp,key,dstStringTemp))
    {
        printf("encrypt error\n");
        return;
    }
    printf("enc %d:",strlen((char*)dstStringTemp));
    for(int i = 0; dstStringTemp[i]; ++i){
        printf("%x",(unsigned char)dstStringTemp[i]);
        tmp.sprintf("%x",(unsigned char)dstStringTemp[i]);
        str.append(tmp);
    }
    qDebug() << "dfaf:" << str;
    ui->textEdit_2->setText(tr("<font size=5 color=red>%1</font>").arg(str));

    QString src = ui->textEdit->toPlainText();
    QString dst = dstStringTemp;

    openDB();
    QSqlQuery qry(m_db);
    QString strSql = QString("insert into info values ('%1','%2','%3')").arg(src).arg(dst).arg(iKey);
    if(!qry.exec(strSql)){
        qDebug() << "insert:" << qry.lastError();
    }
    closeDB();
#endif
}

void MySSl::on_textEdit_textChanged()
{
    ui->textEdit_2->clear();
}

void MySSl::on_btn_decry_clicked()
{
    QString iKey = ui->lineEdit->text();
    if(iKey.isEmpty()){
        QMessageBox::warning(this,tr("警告："),tr("密钥为空！"));
        return;
    }
    QString strSql = QString("select * from info where key='%1'").arg(iKey);
    openDB();
    QSqlQuery qry(m_db);
    if(!qry.exec(strSql)){
        qDebug() << qry.lastError();
        return;
    }
    QString src,dst;
    if (qry.next()) {
        src = qry.value(0).toString();
        dst = qry.value(1).toString();
    }
    qDebug() << "fsdadsf:" << src << dst << iKey;

    //解密
    strcpy((char*)key,iKey.toLatin1().data());
    memset((char*)sourceStringTemp, 0 ,MSG_LEN);
    if(!aes_decrypt(dstStringTemp,key,sourceStringTemp))
    {
        printf("decrypt error\n");
        return;
    }
    printf("\n");
    printf("dec %d:",strlen((char*)sourceStringTemp));
    printf("%s\n",sourceStringTemp);
    ui->textEdit_3->setText(sourceStringTemp);
    for(int i = 0; sourceStringTemp[i]; ++i){
        printf("%x",(unsigned char)sourceStringTemp[i]);
    }
    printf("\n");
}
