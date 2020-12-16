/* taken from qtcontrols/src/eplotlight/plotsaver.h (and relative .cpp)
 * and slightly adapted.
 */

#ifndef PLOTSCENEWIDGET_SAVER_H
#define PLOTSCENEWIDGET_SAVER_H

#include <QList>
#include "scenecurve.h"

#include <QDialog>

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    OptionsDialog(QWidget *parent, double xSample, double ySample);

    QString xFormat() const;
    QString yFormat() const;

    bool dateTimeFormat() const;


    double xSample, ySample;

    QString timestampToDateTimeString(double timestamp, const QString& format);

public slots:
    void setDateTimeFormatEnabled(bool en);

protected slots:
    void updateXExample(const QString& format);
    void updateYExample(const QString& format);
    void setDateTimeFormat(bool timeScale);

private:
    bool mDateTimeFormat;
};

class PlotSceneWidgetSaver
{
  public:
    PlotSceneWidgetSaver();
	
    bool save(const QList<SceneCurve *>curves, bool timeScale);
	
	QString errorMessage() { return d_errorMessage; }
	QString fileName() { return d_fileName; }
	
  private:
	QString d_errorMessage, d_fileName;
};

#endif
