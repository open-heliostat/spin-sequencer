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
	import Select from '$lib/components/Select.svelte';
	import Text from '$lib/components/Text.svelte';
	import Button from '$lib/components/Button.svelte';
	import { getJsonRest, postJsonRest } from '$lib/stores/rest';
	import DisableButton from '$lib/components/DisableButton.svelte';
	import StopButton from '$lib/components/StopButton.svelte';

	const restPath = "/rest/heliostat";

	let newName = "";

	type Direction = {
		azimuth: number;
		elevation: number;
	}

	type HeliostatControllerState = {
		enabled: boolean;
		currentSource: string;
		currentTarget: string;
		sourcesMap: {
			[key: string]: Direction
		}
	}

	let heliostatControllerStateEvent = "heliostat-service"

	let heliostatControllerState : HeliostatControllerState;

	let selectedEditor = "None";
	let selectedDirection: Direction;
	$: selectedDirection = heliostatControllerState?.sourcesMap[selectedEditor];

	let selectedTarget: Direction;
	$: selectedTarget = heliostatControllerState?.sourcesMap[heliostatControllerState.currentTarget];

	let selectedSource: Direction;
	$: selectedSource = heliostatControllerState?.sourcesMap[heliostatControllerState.currentSource];


	onMount(() => {
		// socket.on<HeliostatControllerState>(heliostatControllerStateEvent, (data) => {
		// 	heliostatControllerState = data;
		// 	// console.log(data);
		// });
		// socket.on("heliostat-service", (data) => {
		// 	controlState = Object.assign(controlState, data);
		// 	console.log(data);
		// });
		// socket.sendEvent("heliostat-service", {azimuth:{limits:{enabled:false}}});
	});

	// onDestroy(() => {
	// 	socket.off(heliostatControllerStateEvent);
	// });

	async function getHeliostatControllerState() {
		return getJsonRest(restPath, heliostatControllerState).then((data)=>heliostatControllerState=data);
	}

</script>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Heliostat Control</span>
	<div class="w-full">
		{#await getHeliostatControllerState() then nothing}
		<div class="grid w-full grid-cols-1 content-center gap-x-4 sm:grid-cols-2">
			<div>
				<Select label="Source" bind:value={heliostatControllerState.currentSource} onChange={()=>{postJsonRest(restPath, {currentSource: heliostatControllerState.currentSource})}}>
					<option>None</option>
					{#each Object.entries(heliostatControllerState?.sourcesMap) as [name, value]}
						<option>{name}</option>
					{/each}
				</Select>
			</div>
			<div>
				<Select label="Target" bind:value={heliostatControllerState.currentTarget} onChange={()=>{postJsonRest(restPath, {currentTarget: heliostatControllerState.currentTarget})}}>
					<option>None</option>
					{#each Object.entries(heliostatControllerState?.sourcesMap) as [name, value]}
						<option>{name}</option>
					{/each}
				</Select>
			</div>
		</div>
		<div class="w-full mb-4">
			<Select label="Select" bind:value={selectedEditor} onChange={()=>{newName=selectedEditor}}>
				<option>None</option>
				{#each Object.entries(heliostatControllerState?.sourcesMap) as [name, value]}
					<option>{name}</option>
				{/each}
			</Select>
		</div>
		<GridForm>
			{#if selectedDirection}
			<Text
				label="Name"
				bind:value={newName}
				onChange={()=>{postJsonRest(restPath + "/rename", {oldName: selectedEditor, newName: newName});getHeliostatControllerState()}}
			></Text>
			<Slider 
				label="Azimuth" 
				bind:value={selectedDirection.azimuth}
				min={0} 
				max={360} 
				step={0.01}
				onChange={() => {
					postJsonRest(restPath, {set: {name: selectedEditor, azimuth: selectedDirection.azimuth, elevation: selectedDirection.elevation}});
				}}
			></Slider>
			<Slider 
				label="Elevation" 
				bind:value={selectedDirection.elevation}
				min={0} 
				max={360} 
				step={0.01}
				onChange={() => {
					postJsonRest(restPath, {set: {name: selectedEditor, azimuth: selectedDirection.azimuth, elevation: selectedDirection.elevation}});
				}}
			></Slider>
			{/if}
			<!-- <Checkbox
				label="Enable"
				bind:value={heliostatControllerState.enabled}
			></Checkbox> -->
		</GridForm>
		<div class="flex flex-row flex-wrap justify-between gap-x-2">
			<Button
				label="Add"
				onClick={()=>{postJsonRest(restPath + "/add", {}).then(()=>getHeliostatControllerState())}}>
			</Button>
			{#if selectedDirection}
			<Button
				label="Remove"
				onClick={()=>{postJsonRest(restPath, {remove: selectedEditor}).then(()=>getHeliostatControllerState())}}>
			</Button>
			{/if}
			<div class="flex-grow"></div>
			<DisableButton onClick={() => postJsonRest(restPath, {azimuth:{enabled: false, stepper: {control: {enabled: false}}},elevation:{enabled: false, stepper: {control: {enabled: false}}}})}></DisableButton>
			<StopButton onClick={() => postJsonRest(restPath, {azimuth:{enabled: false, stepper: {control: {stop: {}}}},elevation:{enabled: false, stepper: {control: {stop: {}}}}})}></StopButton>
		</div>
		{/await}
	</div>
</SettingsCard>

<!-- <SettingsCard collapsible={false}>
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
</SettingsCard> -->