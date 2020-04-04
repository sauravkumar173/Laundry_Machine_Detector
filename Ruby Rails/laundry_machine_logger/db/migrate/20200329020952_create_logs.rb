class CreateLogs < ActiveRecord::Migration[6.0]
  def change
    create_table :logs do |t|
      t.string :machine_state
      t.string :log_message
      t.integer :max_x_accl
      t.integer :max_y_accl
      t.integer :max_z_accl

      t.timestamps
    end
  end
end
