Rails.application.routes.draw do
  # For details on the DSL available within this file, see https://guides.rubyonrails.org/routing.html
  get '/logs', to: 'logs#index', as: 'logs'

  # Creates new log and adds to database
  get '/logs/new', to: 'logs#new', as: 'new_log'
  post '/logs', to: 'logs#create', as: 'create_log'
  
  # Read existing log and shows contents
  get '/logs/:id', to: 'logs#show', as: 'log'

  # Edits existing logs with new title and contents
  get '/logs/:id/edit', to: 'logs#edit', as: 'edit_log'
  
  # Updates data base with edited log
  patch '/logs/:id', to: 'logs#update'
  put '/logs/:id', to: 'logs#update'

  # Deletes existing log based on ID
  delete '/logs/:id', to: 'logs#destroy', as: 'destroy_log'
  
  # Send get request to ESP8266 to turn get machine status or acceleration data
  get "/machine/data", to: "logs#accl_data", as: "machine_accl_data"


end
