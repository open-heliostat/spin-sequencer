<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Collapsible from '$lib/components/Collapsible.svelte';
	import Stepper from '~icons/tabler/hand-move';
	import Settings from '~icons/tabler/adjustments';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';
	import Stop from '~icons/tabler/hand-stop';
	import Spinner from '$lib/components/Spinner.svelte';
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import { error } from '@sveltejs/kit';
	import { load } from '../+layout';
	import { socket } from '$lib/stores/socket';
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import Text from '$lib/components/Text.svelte';

	const stepperControlEvent = "steppercontrol"
	const stepperSettingsEvent = "steppersettings"

	type StepperControl = {
		isEnabled: boolean;
		direction: boolean;
		speed: number;
		move: number;
		acceleration: number;
		status: number;
		version: number;
		socketConnected: boolean;
	};

	type SteppersControl = {
		steppers: StepperControl[];
	};

	let steppersControl: SteppersControl = {steppers:[]};

	class StepperStatus {
		stst: boolean = false;
		olb: boolean = false;
		ola: boolean = false;
		s2gb: boolean = false;
		s2ga: boolean = false;
		s2vsb: boolean = false;
		s2vsa: boolean = false;
		otpw: boolean = false;
		ot: boolean = false;
		connected: boolean = false;
		infoclass: string = "";
	}

	let formField: any;

	let stepperStatuses: StepperStatus[];
	$: stepperStatuses = readStatus(steppersControl);

	function readStatus(steppers: SteppersControl) {
		let statuses = [];
		for (var stepper of steppers.steppers) {
			let stepperStatus = new StepperStatus;
			let mainFlags = stepper.status >>> 24;
			stepperStatus.stst = (mainFlags & 0b10000000) > 0;
			stepperStatus.olb = (mainFlags & 0b01000000) > 0;
			stepperStatus.ola = (mainFlags & 0b00100000) > 0;
			stepperStatus.s2gb = (mainFlags & 0b00010000) > 0;
			stepperStatus.s2ga = (mainFlags & 0b00001000) > 0;
			stepperStatus.otpw = (mainFlags & 0b00000100) > 0;
			let otherFlags = stepper.status >>> 24;
			stepperStatus.s2vsb = (otherFlags & 0b10) > 0;
			stepperStatus.s2vsa = (otherFlags & 0b01) > 0;
			stepperStatus.connected = !(stepper.version == 0 || stepper.version == 255);
			stepperStatus.infoclass = stepperStatus.connected ? ((stepperStatus.ola || stepperStatus.olb || stepperStatus.otpw || stepperStatus.ot || stepperStatus.s2ga || stepperStatus.s2gb || stepperStatus.s2vsa || stepperStatus.s2vsb) ? 'alert-warning' : 'alert-info') : 'alert-warning';
			statuses.push(stepperStatus);
		}
		return statuses;
	}

	onMount(() => {
		socket.on<SteppersControl>(stepperControlEvent, (data) => {
			steppersControl = data;
			socketConnected = true;
		});
		socket.on<MultiStepperSettings>(stepperSettingsEvent, (data) => {
			stepperSettings = data;
		});
	});

	onDestroy(() => {
		socket.off(stepperControlEvent);
		socket.off(stepperSettingsEvent);
	});

	let socketConnected = false;

	type StepperSettings = {
		name: string;
		enableOnStart: boolean;
		invertDirection: boolean;
		maxSpeed: number;
		maxAcceleration: number;
		current: number;
	};
	type MultiStepperSettings = {
		steppers: StepperSettings[]
	}

	let stepperSettings: MultiStepperSettings;

</script>


<style>
	.grid-form {
	  grid-template-columns: [labels] auto [controls] 1fr;
	}
</style>

{#each steppersControl.steppers as stepper, i}
<SettingsCard collapsible={true} open={true}>
	<Stepper slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">{stepperSettings?.steppers[i].name}</span>
	<div class="w-full">
		{#if socketConnected}
		<div class="alert {stepperStatuses[i].infoclass} my-2 shadow-lg">
			<Info class="h-6 w-6 flex-shrink-0 stroke-current" />
			<span>
				Stepper driver {stepperStatuses[i].connected ? (stepper.isEnabled ? 'enabled' : 'disabled') : 'disconnected'} !
				{#if !stepperStatuses[i].connected}
				<br> Communication error, check the driver and power supply connexions.
				{:else}
					{#if stepperStatuses[i].ola || stepperStatuses[i].olb}
					<br> Open load detected, check motor connexions.
					{/if}
					{#if stepperStatuses[i].s2ga || stepperStatuses[i].s2gb}
					<br> Short circuit to ground detected, check motor connexions.
					{/if}
					{#if stepperStatuses[i].s2vsa || stepperStatuses[i].s2vsb}
					<br> Short circuit to supply detected, check motor connexions.
					{/if}
					{#if stepperStatuses[i].ot}
					<br> Driver is too hot, let it cool down and retry.
					{:else if stepperStatuses[i].otpw}
					<br> Driver temperature pre-warning, you may want to try lowering the current and/or reducing the load.
					{/if}
				{/if}
			</span>
		</div>
		<div class="w-full grid grid-flow-row grid-form items-center">
			<Checkbox
				label="Enable"
				bind:value={stepper.isEnabled}
				onChange={() => {socket.sendEvent(stepperControlEvent, steppersControl);}}
			></Checkbox>
			<Checkbox
				label="Direction"
				bind:value={stepper.direction}
				onChange={() => {socket.sendEvent(stepperControlEvent, steppersControl);}}
			></Checkbox>
			<Slider
				label="Speed"
				min={0}
				max={1}
				step={0.01}
				bind:value={stepper.speed}
				onChange={() => {socket.sendEvent(stepperControlEvent, steppersControl);}}
			></Slider>
			<Slider
				label="Move"
				min={0}
				max={1}
				step={0.01}
				bind:value={stepper.move}
			></Slider>
			<Slider
				label="Acceleration"
				min={0}
				max={1}
				step={0.01}
				bind:value={stepper.acceleration}
				onChange={() => {socket.sendEvent(stepperControlEvent, steppersControl);}}
			></Slider>
		</div>
		<div class="flex flex-row flex-wrap justify-between gap-x-2">
			<div class="flex-grow"></div>
			<div>
				<div>
					<button class="btn btn-primary inline-flex items-center" on:click={() => {stepper.speed=0; socket.sendEvent(stepperControlEvent, steppersControl);}}
						><Stop class="mr-2 h-5 w-5" /><span>Stop</span></button
					>
				</div>
			</div>
		</div>
		{:else}
		<div class="alert alert-warning my-2 shadow-lg">
			<Info class="h-6 w-6 flex-shrink-0 stroke-current" />
			<span>
				WebSocket Disconnected !
				<br>
				Too many clients or device unreacheable.
			</span>
		</div>
		{/if}
	</div>

	<Collapsible open={false}>
		<Settings slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
		<span slot="title">Settings</span>
		<div class="w-full overflow-x-auto">
				<form
				>
					<div class="w-full">
						<div class="w-full grid grid-flow-row grid-form items-center">
							<Text 
								label="Name" 
								bind:value={stepperSettings.steppers[i].name} 
								onChange={() => socket.sendEvent(stepperSettingsEvent, stepperSettings)}
							></Text>
							<Checkbox
								label="Enable on Start"
								bind:value={stepperSettings.steppers[i].enableOnStart}
								onChange={() => socket.sendEvent(stepperSettingsEvent, stepperSettings)}
							></Checkbox>
							<Checkbox
								label="Invert Direction"
								bind:value={stepperSettings.steppers[i].invertDirection}
								onChange={() => socket.sendEvent(stepperSettingsEvent, stepperSettings)}
							></Checkbox>
							<Slider
								label="Max Speed"
								min={0}
								max={400}
								bind:value={stepperSettings.steppers[i].maxSpeed}
								onChange={() => socket.sendEvent(stepperSettingsEvent, stepperSettings)}
							></Slider>
							<Slider
								label="Max Acceleration"
								min={0}
								max={200}
								bind:value={stepperSettings.steppers[i].maxAcceleration}
								onChange={() => socket.sendEvent(stepperSettingsEvent, stepperSettings)}
							></Slider>
							<Slider
								label="Driver Current"
								min={0}
								max={4000}
								bind:value={stepperSettings.steppers[i].current}
								onChange={() => socket.sendEvent(stepperSettingsEvent, stepperSettings)}
							></Slider>
						</div>
					</div>
				</form>
		</div>
	</Collapsible>

</SettingsCard>
{/each}