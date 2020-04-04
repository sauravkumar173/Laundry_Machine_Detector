class AddLogXAcclToLogs < ActiveRecord::Migration[6.0]
  def change
    add_column :logs, :log_x_accl, :Integer
    add_column :logs, :log_y_accl, :Integer
    add_column :logs, :log_z_accl, :Integer
    rename_column :logs, :max_x_accl, :x_accl
    rename_column :logs, :max_y_accl, :y_accl
    rename_column :logs, :max_z_accl, :z_accl
  end
end
