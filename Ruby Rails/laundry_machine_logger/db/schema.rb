# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# This file is the source Rails uses to define your schema when running `rails
# db:schema:load`. When creating a new database, `rails db:schema:load` tends to
# be faster and is potentially less error prone than running all of your
# migrations from scratch. Old migrations may fail to apply correctly if those
# migrations use external dependencies or application code.
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 2020_03_29_102643) do

  create_table "logs", force: :cascade do |t|
    t.string "machine_state"
    t.string "log_message"
    t.integer "x_accl"
    t.integer "y_accl"
    t.integer "z_accl"
    t.datetime "created_at", precision: 6, null: false
    t.datetime "updated_at", precision: 6, null: false
    t.integer "log_x_accl"
    t.integer "log_y_accl"
    t.integer "log_z_accl"
  end

end
