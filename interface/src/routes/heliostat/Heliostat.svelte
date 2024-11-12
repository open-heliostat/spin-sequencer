<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Light from '~icons/tabler/bulb';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';
	import { socket } from '$lib/stores/socket';
	import GridForm from '$lib/components/GridForm.svelte';
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import type { ControllerState } from '$lib/types/models'
	import ControllerSettings from './ControllerSettings.svelte';

	type HeliostatControllerState = {
		azimuth: number;
		elevation: number;
	}

	let heliostatControllerStateEvent = "heliostat-control"

	let heliostatControllerState : HeliostatControllerState;


	type HeliostatControlState = {
		azimuth?: ControllerState;
		elevation?: ControllerState;
	}

	let controlState : HeliostatControlState = {};

	onMount(() => {
		socket.on<HeliostatControllerState>(heliostatControllerStateEvent, (data) => {
			heliostatControllerState = data;
		});
		socket.on("heliostat-service", (data) => {
			controlState = Object.assign(controlState, data);
			console.log(data);
		});
		socket.sendEvent("heliostat-service", {azimuth:{limits:{enabled:false}}});
	});

	onDestroy(() => {
		socket.off(heliostatControllerStateEvent);
	});

</script>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Heliostat Control</span>
	{#if heliostatControllerState}
	<div class="w-full">
		<Slider 
			label="Azimuth" 
			bind:value={heliostatControllerState.azimuth}
			min={0} 
			max={360} 
			step={0.01}
			onChange={() => socket.sendEvent(heliostatControllerStateEvent, heliostatControllerState)}
		></Slider>
		<Slider 
			label="Elevation" 
			bind:value={heliostatControllerState.elevation}
			min={0} 
			max={360} 
			step={0.01}
			onChange={() => socket.sendEvent(heliostatControllerStateEvent, heliostatControllerState)}
		></Slider>
	</div>
	{/if}
</SettingsCard>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Settings</span>
	<ControllerSettings
		label="Azimuth"
		controllerState={controlState.azimuth}
		hydrate={() => socket.sendEvent("heliostat-service", {azimuth: {}})}
		sendEvent={(x) => {socket.sendEvent("heliostat-service",{azimuth: x});}}
	>
	</ControllerSettings>
	<ControllerSettings 
		label="Elevation"
		controllerState={controlState.elevation} 
		hydrate={() => socket.sendEvent("heliostat-service", {elevation: {}})}
		sendEvent={(x) => {socket.sendEvent("heliostat-service",{elevation: x});}}
	>
	</ControllerSettings>
</SettingsCard>