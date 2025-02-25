export type WifiStatus = {
	status: number;
	local_ip: string;
	mac_address: string;
	rssi: number;
	ssid: string;
	bssid: string;
	channel: number;
	subnet_mask: string;
	gateway_ip: string;
	dns_ip_1: string;
	dns_ip_2?: string;
};

export type WifiSettings = {
	hostname: string;
	connection_mode: number;
	wifi_networks: KnownNetworkItem[];
};

export type KnownNetworkItem = {
	ssid: string;
	password: string;
	static_ip_config: boolean;
	local_ip?: string;
	subnet_mask?: string;
	gateway_ip?: string;
	dns_ip_1?: string;
	dns_ip_2?: string;
};

export type NetworkItem = {
	rssi: number;
	ssid: string;
	bssid: string;
	channel: number;
	encryption_type: number;
};

export type ApStatus = {
	status: number;
	ip_address: string;
	mac_address: string;
	station_num: number;
};

export type ApSettings = {
	provision_mode: number;
	ssid: string;
	password: string;
	channel: number;
	ssid_hidden: boolean;
	max_clients: number;
	local_ip: string;
	gateway_ip: string;
	subnet_mask: string;
};

export type LightState = {
	led_on: boolean;
	red: number;
	green: number;
	blue: number;
};

export type BrokerSettings = {
	mqtt_path: string;
	name: string;
	unique_id: string;
};

export type NTPStatus = {
	status: number;
	utc_time: string;
	local_time: string;
	server: string;
	uptime: number;
};

export type NTPSettings = {
	enabled: boolean;
	server: string;
	tz_label: string;
	tz_format: string;
};

export type GPSSettings = {
	enabled: boolean;
}

export type GPSStatus = {
	latitude: number;
	longitude: number;
	altitude: number;
	numSats: number;
	timeStr: string;
	dateStr: string;
	fixQuality: number;
	hasSerial: boolean;
}

export type Analytics = {
	max_alloc_heap: number;
	psram_size: number;
	free_psram: number;
	free_heap: number;
	total_heap: number;
	min_free_heap: number;
	core_temp: number;
	fs_total: number;
	fs_used: number;
	uptime: number;
};

export type RSSI = {
	rssi: number;
	ssid: string;
};

export type Battery = {
	soc: number;
	charging: boolean;
};

export type DownloadOTA = {
	status: string;
	progress: number;
	error: string;
};

export type StaticSystemInformation = {
	esp_platform: string;
	firmware_version: string;
	cpu_freq_mhz: number;
	cpu_type: string;
	cpu_rev: number;
	cpu_cores: number;
	sketch_size: number;
	free_sketch_space: number;
	sdk_version: string;
	arduino_version: string;
	flash_chip_size: number;
	flash_chip_speed: number;
	cpu_reset_reason: string;
};

export type SystemInformation = Analytics & StaticSystemInformation;

export type MQTTStatus = {
	enabled: boolean;
	connected: boolean;
	client_id: string;
	last_error: string;
};

export type MQTTSettings = {
	enabled: boolean;
	uri: string;
	username: string;
	password: string;
	client_id: string;
	keep_alive: number;
	clean_session: boolean;
};

export type StepperSettings = {
	name: string;
	enableOnStart: boolean;
	invertDirection: boolean;
	maxSpeed: number;
	maxAcceleration: number;
	current: number;
	stepsPerRot: number;
};

export type MultiStepperSettings = {
	steppers: StepperSettings[]
}

export type StepperControl = {
	isEnabled: boolean;
	direction: boolean;
	speed: number;
	move: number;
	acceleration: number;
	status: number;
	version: number;
};

export type StepperControlState = {
	speed: number;
	move: number;
	accel: number;
}

export type StepperConfig = {
	enabled: boolean;
	maxSpeed: number;
	maxAccel: number;
	invertDirection: boolean;
	driverCurrent: number;
	stepsPerRot: number;
}

export type StepperDiag = {
	status: number;
	version: number;
	isEnabled: boolean;
}

export type MultiStepperControl = {
	steppers: StepperControl[];
};

export type ControllerState = {
	position: number;
	target: number;
	tolerance: number;
	offset: number;
	enabled: boolean;
	invert: boolean;
	encoderError: boolean;
	limits: {
		enabled: boolean;
		begin: number;
		end: number;
	};
	calibration: {
		enabled: boolean;
		running: boolean;
		steps: number;
		speed: number;
		decay: number;
	};
}

export type StepperDriver = {
  status: {
    enabled: boolean;
    stallguard: boolean;
    overtemp: boolean;
    overtemp_warning: boolean; 
    cs_actual: number;
    current_scale: number;
  };
  chopConfig: {
    toff: number;
    hstrt: number;
    hend: number;
    tbl: number;
    mres: number;
  };
  stallConfig: {
    sg_stop: boolean;
    semin: number;
    semax: number;
    sedn: number;
    sgt: number;
  };
  pwmConfig: {
    pwm_ofs: number;
    pwm_grad: number;
    pwm_freq: number;
    pwm_autoscale: boolean;
    pwm_autograd: boolean;
  };
};