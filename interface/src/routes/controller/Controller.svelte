<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Hand from '~icons/tabler/hand-move';
	import Settings from '~icons/tabler/adjustments';
	import { socket } from '$lib/stores/socket';
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import Text from '$lib/components/Text.svelte';
	import Collapsible from '$lib/components/Collapsible.svelte';

	const closedLoopControllerEvent = "controller"
	const closedLoopSettingsEvent = "controllersettings"

	type ClosedLoopController = {
		targetAngle: number;
		curAngle: number;
	}
	type ClosedLoopControllers = {
		controllers: ClosedLoopController[];
	}

	type ClosedLoopSettings = {
		tolerance: number;
		enabled: boolean;
		hasLimits: boolean;
		limitA: number;
		limitB: number;
		name: string;
	}
	type ClosedLoopSettingsMulti = {
		controllers: ClosedLoopSettings[];
	}

	let closedLoopControllers: ClosedLoopControllers = {controllers: []};
	let closedLoopSettings: ClosedLoopSettingsMulti = {controllers: []};

	onMount(() => {
		socket.on<ClosedLoopControllers>(closedLoopControllerEvent, (data) => {
			closedLoopControllers = data;
		});
		socket.on<ClosedLoopSettingsMulti>(closedLoopSettingsEvent, (data) => {
			closedLoopSettings = data;
		});
	});

	onDestroy(() => {
		socket.off(closedLoopControllerEvent);
		socket.off(closedLoopSettingsEvent);
	});

</script>


<style>
	.grid-form {
	  grid-template-columns: [labels] auto [controls] 1fr;
	}
</style>

{#each closedLoopSettings.controllers as controller, i}
<SettingsCard collapsible={true} open={true}>
	<Hand slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">{controller.name}</span>
	<div class="w-full grid grid-flow-row grid-form items-center">
		<Slider 
			label="Target" 
			bind:value={closedLoopControllers.controllers[i].targetAngle}
			min={0} 
			max={360} 
			step={0.01}
			onChange={() => socket.sendEvent(closedLoopControllerEvent, closedLoopControllers)}
		></Slider>
		<Slider 
			label="Position" 
			bind:value={closedLoopControllers.controllers[i].curAngle}
			min={0} 
			max={360} 
			step={0.01}
			disabled
			onChange={() => socket.sendEvent(closedLoopControllerEvent, closedLoopControllers)}
		></Slider>
	</div>

	<Collapsible open={false}>
		<Settings slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
		<span slot="title">Settings</span>
		<div class="w-full grid grid-flow-row grid-form items-center">
			<Text
				label="Name"
				bind:value={controller.name}
			></Text>
			<Checkbox 
				label="Control"
				bind:value={controller.enabled}
				onChange={() => socket.sendEvent(closedLoopSettingsEvent, closedLoopSettings)}
			></Checkbox>
			<Slider 
				label="Tolerance" 
				bind:value={controller.tolerance}
				min={0.04} 
				onChange={() => socket.sendEvent(closedLoopSettingsEvent, closedLoopSettings)}
			></Slider>
			<Checkbox 
				label="Limits"
				bind:value={controller.hasLimits}
				onChange={() => socket.sendEvent(closedLoopSettingsEvent, closedLoopSettings)}
			></Checkbox>
			{#if controller.hasLimits}
			<Slider 
				label="Limit A" 
				bind:value={controller.limitA}
				min={0} 
				max={360} 
				step={0.01}
				onChange={() => socket.sendEvent(closedLoopSettingsEvent, closedLoopSettings)}
			></Slider>
			<Slider 
				label="Limit B" 
				bind:value={controller.limitB}
				min={0} 
				max={360} 
				step={0.01}
				onChange={() => socket.sendEvent(closedLoopSettingsEvent, closedLoopSettings)}
			></Slider>
			{/if}
		</div>
	</Collapsible>
</SettingsCard>
{/each}