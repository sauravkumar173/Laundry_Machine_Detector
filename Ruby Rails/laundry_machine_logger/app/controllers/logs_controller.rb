class LogsController < ApplicationController
    # Authenticity token used to stop dangerous http requests
    skip_before_action :verify_authenticity_token

    #retreives logs from the database based and sorts in descending  
    #order of date created
    def index
        @log = Log.order(created_at: :desc)
    end

    def new
        @log = Log.new
    end

    # Creates a new log and ensures it is valid
    def create
        @log = Log.new(log_params)

        if @log.valid? && @log.save
            redirect_to logs_path
        else
            render :new
        end
    end

    # Retrieves log with a particular id
    def show
        @log = Log.find(params[:id])
    end

    # Retrieves post with a particular id to be edited
    def edit
        @log = Log.find(params[:id])
    
    end

    #updates a specific log's title and contents
    def update
        #retrieves log and updates the parameters
        @log = Log.find(params[:id])
        @log.update!(log_params)

        #ensures log is still valid
        if @log.valid? && @log.save
            redirect_to logs_path
        else
            render :edit
        end

    end

    #deletes a log based on id
    def destroy
        Log.destroy(params[:id])
        redirect_to logs_path
    end

    #sends a get request to ESP8266 and retrieves JSON data for the machine
    def accl_data
        @response = HTTParty.get("http://192.168.20.30/machine/data")
        render json: @response
    end


    private
    #private function to retrieve log parameters including title and contents
    def log_params
        params.require(:log).permit(:machine_state, :log_message, :log_x_accl, :log_y_accl, :log_z_accl)
    end

end
