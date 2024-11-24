<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Stepper from '~icons/tabler/hand-move';
	import { socket } from '$lib/stores/socket';
	import type { MultiStepperSettings, MultiStepperControl } from '$lib/types/models';
	import StepperStatusComp from '$lib/components/StepperStatusComp.svelte';
	import StepperControlComp from '$lib/components/StepperControlComp.svelte';
	import StepperSettingsComp from '$lib/components/StepperSettingsComp.svelte';
	import StepperRestComp from '$lib/components/StepperRestComp.svelte';

	const stepperControlEvent = "steppercontrol"
	const stepperSettingsEvent = "steppersettings"

	let steppersControl: MultiStepperControl = {steppers:[]};

	let steppersSettings: MultiStepperSettings;

	onMount(() => {
	});

	onDestroy(() => {
	});

</script>

<SettingsCard>
	<span slot="title">Azimuth Stepper</span>
	<StepperRestComp restPath={"/rest/heliostat/azimuth/stepper"}></StepperRestComp>
</SettingsCard>
<SettingsCard>
	<span slot="title">Azimuth Stepper</span>
	<StepperRestComp restPath={"/rest/heliostat/elevation/stepper"}></StepperRestComp>
</SettingsCard>