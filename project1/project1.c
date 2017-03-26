/*******************************************************************************
* rc_project_template.c
*
* This is meant to be a skeleton program for robotics cape projects.
* Change this description and file name
*******************************************************************************/

// usefulincludes is a collection of common system includes for the lazy
// This is not necessary for roboticscape projects but here for convenience
#include <rc_usefulincludes.h>
// main roboticscape API header
#include <roboticscape.h>

// choice of 1,2,4,8,16 oversampling. Here we use 16 and sample at 25hz which
// is close to the update rate specified in robotics_cape.h for that oversample.
#define OVERSAMPLE  BMP_OVERSAMPLE_8
// choice of OFF, 2, 4, 8, 16 filter constants. Here we turn off the filter and
// opt to use our own 2nd order filter instead.
#define INTERNAL_FILTER	BMP_FILTER_16

// function declarations
void on_pause_pressed();
void on_pause_released();
void print_mag_data();
void print_gyro_data();
void print_accel_data();
void print_barometer_data();
/*******************************************************************************
* int main()
*
* This template main function contains these critical components
* - call to rc_initialize() at the beginning
* - main while loop that checks for EXITING condition
* - rc_cleanup() at the end
*******************************************************************************/
int main(){
	// always initialize cape library first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}
	// do your own initialization here
	printf("\nHello BeagleBone\n");
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);

	rc_imu_config_t conf = rc_default_imu_config();
	conf.enable_magnetometer=1;
	rc_imu_data_t imu_data;

	if(rc_initialize_barometer(OVERSAMPLE, INTERNAL_FILTER)<0){
		fprintf(stderr, "ERROR: rc_initialize_barometer failed \n");
		return -1;
	}


	if(rc_initialize_imu(&imu_data, conf)){

	}
	// done initializing so set state to RUNNING
	rc_set_state(RUNNING);

	// Keep looping until state changes to EXITING
	while(rc_get_state()!=EXITING){
		// handle other states
		if(rc_get_state()==RUNNING){
			// do things
			rc_blink_led(GREEN, 10, 1);
			print_mag_data(&imu_data);
			print_gyro_data(&imu_data);
			print_accel_data(&imu_data);
			print_barometer_data();
			fflush(stdout);
		}
		else if(rc_get_state()==PAUSED){
			// do other things
			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);
		}
		// always sleep at some point
		usleep(100000);
	}

	// exit cleanly
	rc_cleanup();
	return 0;
}


/*******************************************************************************
* void on_pause_released()
*
* Make the Pause button toggle between paused and running states.
*******************************************************************************/
void on_pause_released(){
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING)		rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}

/*******************************************************************************
* void on_pause_pressed()
*
* If the user holds the pause button for 2 seconds, set state to exiting which
* triggers the rest of the program to exit cleanly.
*******************************************************************************/
void on_pause_pressed(){
	int i=0;
	const int samples = 100;	// check for release 100 times in this period
	const int us_wait = 2000000; // 2 seconds

	// now keep checking to see if the button is still held down
	for(i=0;i<samples;i++){
		rc_usleep(us_wait/samples);
		if(rc_get_pause_button() == RELEASED) return;
	}
	printf("long press detected, shutting down\n");
	rc_set_state(EXITING);
	return;
}

void print_mag_data(rc_imu_data_t *data){
	if(rc_read_mag_data(data)<0){
		printf("read mag data failed\n");
	}
	else printf("mag: %6.1f %6.1f %6.1f |\n", (*data).mag[0],
					   (*data).mag[1],
					   (*data).mag[2]);
}

void print_gyro_data(rc_imu_data_t *data){
	if(rc_read_gyro_data(data)<0){
		printf("read gyro data failed\n");
	}
	else printf("gyro: %6.1f %6.1f %6.1f |\n", (*data).gyro[0],
					   (*data).gyro[1],
					   (*data).gyro[2]);
}

void print_accel_data(rc_imu_data_t *data){
	if(rc_read_accel_data(data)<0){
		printf("read accel data failed\n");
	}
	else printf("accel: %6.1f %6.1f %6.1f |\n", (*data).accel[0],
						 (*data).accel[1],
						 (*data).accel[2]);
}

void print_barometer_data(){
	double temp, pressure, altitude;
	if(rc_read_barometer()<0){
		fprintf(stderr, "\rError: Can't read Barometer");
	}

		temp = rc_bmp_get_temperature();
		pressure = rc_bmp_get_pressure_pa();
		altitude = rc_bmp_get_altitude_m();

		printf("\r");
		printf("%6.2fC |", temp);
		printf("%7.2fkpa |", pressure/1000.0);
		printf("%8.2fm |\n", altitude);
		fflush(stdout);
}
