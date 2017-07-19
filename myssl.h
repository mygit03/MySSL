#ifndef MYSSL_H
#define MYSSL_H

#include <QWidget>

#include <QSqlDatabase>

namespace Ui {
class MySSl;
}

class MySSl : public QWidget
{
    Q_OBJECT

public:
    explicit MySSl(QWidget *parent = 0);
    ~MySSl();

    std::string sha256(const std::string str);      //加密

    int aes_encrypt(char* in, char* key, char* out);        //加密
    int aes_decrypt(char* in, char* key, char* out);        //解密

    void openDB();
    void closeDB();

private slots:
    void on_btn_encry_clicked();

    void on_textEdit_textChanged();

    void on_btn_decry_clicked();

private:
    Ui::MySSl *ui;

    QSqlDatabase m_db;
};

#endif // MYSSL_H
