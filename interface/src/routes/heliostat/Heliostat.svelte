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
	import ControllerSettings from '../../lib/components/ControllerSettings.svelte';

	type Direction = {
		azimuth: number;
		elevation: number;
	}

	type HeliostatControllerState = {
		currentSource: string;
		currentTarget: string;
		sourcesMap: {
			[key: string]: Direction
		}
		targetsMap: {
			[key: string]: Direction
		}
	}

	let heliostatControllerStateEvent = "heliostat-service"

	let heliostatControllerState : HeliostatControllerState;

	let selectedTarget: Direction;
	$: selectedTarget = heliostatControllerState?.targetsMap[heliostatControllerState.currentTarget];

	let selectedSource: Direction;
	$: selectedSource = heliostatControllerState?.sourcesMap[heliostatControllerState.currentSource];


	onMount(() => {
		socket.on<HeliostatControllerState>(heliostatControllerStateEvent, (data) => {
			heliostatControllerState = data;
			// console.log(data);
		});
		// socket.on("heliostat-service", (data) => {
		// 	controlState = Object.assign(controlState, data);
		// 	console.log(data);
		// });
		// socket.sendEvent("heliostat-service", {azimuth:{limits:{enabled:false}}});
	});

	onDestroy(() => {
		socket.off(heliostatControllerStateEvent);
	});

</script>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Heliostat Control</span>
	{#if selectedTarget}
	<div class="w-full">
		<Slider 
			label="Azimuth" 
			bind:value={selectedTarget.azimuth}
			min={0} 
			max={360} 
			step={0.01}
			onChange={() => {
				heliostatControllerState.targetsMap[heliostatControllerState.currentTarget] = selectedTarget;
				socket.sendEvent(heliostatControllerStateEvent, heliostatControllerState);
			}}
		></Slider>
		<Slider 
			label="Elevation" 
			bind:value={selectedTarget.elevation}
			min={0} 
			max={360} 
			step={0.01}
			onChange={() => {
				heliostatControllerState.targetsMap[heliostatControllerState.currentTarget] = selectedTarget;
				socket.sendEvent(heliostatControllerStateEvent, heliostatControllerState);
			}}
		></Slider>
	</div>
	{/if}
</SettingsCard>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Settings</span>
	<ControllerSettings
		label="Azimuth"
		restPath="/rest/heliostat/azimuth"
	>
	</ControllerSettings>
	<ControllerSettings 
		label="Elevation"
		restPath="/rest/heliostat/elevation"
	>
	</ControllerSettings>
</SettingsCard>