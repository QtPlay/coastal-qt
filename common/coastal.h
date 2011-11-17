#include <QIcon>
#include <QDialog>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>

class CoastalAbout : public QDialog
{
Q_OBJECT

protected:
    friend class CoastalMain;

    CoastalAbout(QWidget *parent);

    void setImage(QGraphicsScene& scene);
    void setVersion(const QString& text);
    void setName(const QString& text);
    void setCopyright(const QString& text);
};

class CoastalMain : public QMainWindow
{
Q_OBJECT

public:
    CoastalMain();

public slots:
    void about(void);

private:
    const char *program_version, *program_name, *program_copyright;
};

