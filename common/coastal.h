#include <QIcon>
#include <QDialog>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QProcess>
#include <QDebug>

class CoastalAbout : public QDialog
{
Q_OBJECT

protected:
    friend class CoastalMain;

    CoastalAbout(QWidget *parent);

    void setImage(QGraphicsScene& scene);
    void setVersion(const QString& text);
    void setAbout(const QString& text);
    void setCopyright(const QString& text);
};

class CoastalMain : public QMainWindow
{
Q_OBJECT

protected:
    const char *program_version, *program_about, *program_copyright, *program_name;
    const char *url_support;

    CoastalMain();

public slots:
    void about(void);
    void support(void);
};

class Coastal
{
public:
    static bool env(const char *id, char *buffer, size_t size);

    static bool browser(const char *url);

    static bool open(const char *filename);
};


