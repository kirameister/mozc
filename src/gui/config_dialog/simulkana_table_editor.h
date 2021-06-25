#ifndef MOZC_GUI_CONFIG_DIALOG_SIMULKANA_TABLE_EDITOR_H_
#define MOZC_GUI_CONFIG_DIALOG_SIMULKANA_TABLE_EDITOR_H_

#include <QtWidgets/QWidget>
#include <memory>
#include <string>

#include "base/port.h"
#include "gui/config_dialog/generic_table_editor.h"

class QAbstractButton;

namespace mozc {
namespace gui {

class SimulKanaTableEditorDialog : public GenericTableEditorDialog {
  Q_OBJECT;

 public:
  explicit SimulKanaTableEditorDialog(QWidget *parent);
  ~SimulKanaTableEditorDialog() override;

  // show a modal dialog
  static bool Show(QWidget *parent, const std::string &current_simulkana_table,
                   std::string *new_simulkana_table);

 protected slots:
  void UpdateMenuStatus() override;
  void OnEditMenuAction(QAction *action) override;

 protected:
  std::string GetDefaultFilename() const override { return "shingeta_table.txt"; }
  bool LoadFromStream(std::istream *is) override;
  bool Update() override;

 private:
  bool LoadDefaultSimulKanaTable();
  static std::string GetDefaultSimulKanaTable();

 private:
  std::unique_ptr<QAction *[]> actions_;
  QString dialog_title_;
};

}  // namespace gui
}  // namespace mozc

#endif  // MOZC_GUI_CONFIG_DIALOG_SIMULKANA_TABLE_EDITOR_H_
