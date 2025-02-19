<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import StepperRestComp from '$lib/components/StepperRestComp.svelte';

	onMount(() => {
	});

	onDestroy(() => {
	});

</script>

<SettingsCard>
	<span slot="title">Stepper</span>
	<StepperRestComp restPath={"/rest/spin-seq/controller/stepper"}></StepperRestComp>
</SettingsCard>