// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file mainwindow.h
  @author R. Boudewijns

  This file contains the main window of LTSGraph.

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

#include "mcrl2/utilities/persistentfiledialog.h"

#include "glwidget.h"
#include "graph.h"
#include "springlayout.h"
#include "information.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor.
     * @param parent The optional parent for the user interface.
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~MainWindow();

    /**
     * @brief Opens the given file if it is valid.
     */
    void openFile(QString fileName);

    /**
     * @brief Opens the given file if it is valid.
     */
    void delayedOpenFile(QString fileName);

  private slots:

    /**
     * @brief Load command-line passed file only after OpenGL is initialized.
     */
    void onOpenGLInitialized();

    /**
     * @brief Displays a open file dialog in which a graph can be selected which will be loaded.
     */
    void onOpenFile();

    /**
     * @brief Displays an Image export dialog and saves the current visualisation to the selected file.
     */
    void onExportImage();

    /**
     * @brief Displays an XML import dialog and loads the selected file in the graph.
     */
    void onImportXML();

    /**
     * @brief Displays an XML export dialog and saves the current graph to the selected file.
     */
    void onExportXML();

    /**
     * @brief Updates the Graph and SpringLayout such that the graph fits in the given @e newsize.
     * @param newsize The new size of the GLWidget.
     */
    void onWidgetResized(const Graph::Coord3D& newsize);

    /**
     * @brief Enables or disables the 3D mode of the GLWidget.
     * @param enabled Indicates wether the 3D mode should be enabled.
     */
    void on3DChanged(bool enabled);

    /**
     * @brief Updates the Graph and GLWidget for the current time. Should be called periodically.
     */
    void onTimer();

    /**
     * @brief Updates the statusbar with the latest log output
     */
    void onLogOutput(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

  protected:
    /**
     * @brief Saves window information
     */
    void closeEvent(QCloseEvent *event);

  private:
    Ui::MainWindow m_ui;               ///< The user interface generated by Qt.
    GLWidget* m_glwidget;               ///< The GLWidget containing the visualisation of the graph.
    Graph::Graph m_graph;               ///< The current Graph.
    Graph::SpringLayout* m_layout;      ///< The SpringLayout algorithm which is applied to the graph.
    Graph::Information* m_information;  ///< The Information display which calculates the statistics of the graph.
    QTimer* m_timer;                    ///< The times used to call onTimer periodically.
    QString m_delayedOpen;              ///< The file that should be opened when the program is ready

    mcrl2::utilities::qt::PersistentFileDialog m_fileDialog;

};

#endif // MAINWINDOW_H
