<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Stepper from '~icons/tabler/hand-move';
	import { socket } from '$lib/stores/socket';
	import type { MultiStepperSettings, MultiStepperControl } from '$lib/types/models';
	import StepperStatusComp from '$lib/components/StepperStatusComp.svelte';
	import StepperControlComp from '$lib/components/StepperControlComp.svelte';
	import StepperSettingsComp from '$lib/components/StepperSettingsComp.svelte';

	const stepperControlEvent = "steppercontrol"
	const stepperSettingsEvent = "steppersettings"

	let steppersControl: MultiStepperControl = {steppers:[]};

	let steppersSettings: MultiStepperSettings;

	onMount(() => {
		socket.on<MultiStepperControl>(stepperControlEvent, (data) => {
			steppersControl = data;
		});
		socket.on<MultiStepperSettings>(stepperSettingsEvent, (data) => {
			steppersSettings = data;
		});
	});

	onDestroy(() => {
		socket.off(stepperControlEvent);
		socket.off(stepperSettingsEvent);
	});

</script>

{#each steppersControl.steppers as stepperControl, i}
<SettingsCard collapsible={true} open={true}>
	<Stepper slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">{steppersSettings?.steppers[i].name}</span>
	<div class="w-full">
		<StepperStatusComp {stepperControl}></StepperStatusComp>
		<StepperControlComp 
			{stepperControl}
			onChange={() => socket.sendEvent(stepperControlEvent, steppersControl)}
		></StepperControlComp>
		<StepperSettingsComp
			stepperSettings={steppersSettings?.steppers[i]}
			onChange={() => socket.sendEvent(stepperSettingsEvent, steppersSettings)}
		></StepperSettingsComp>
	</div>
</SettingsCard>
{/each}