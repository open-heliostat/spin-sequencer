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
	{#snippet title()}
		<span>Stepper</span>
	{/snippet}
	<StepperRestComp restPath={"/rest/spin-seq/controller/stepper"}></StepperRestComp>
</SettingsCard>