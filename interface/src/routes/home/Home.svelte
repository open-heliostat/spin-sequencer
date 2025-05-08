<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import StepperRestComp from '$lib/components/StepperRestComp.svelte';
	import CanConsoleRestComp from '$lib/components/CANConsoleRestComp.svelte';
	import CanSettingsRestComp from '$lib/components/CANSettingsRestComp.svelte';
	import SequencerRestComp from '$lib/components/SequencerRestComp.svelte';
	import { getJsonRest } from '$lib/stores/rest';
	import StepperStatusComp from '$lib/components/StepperStatusComp.svelte';
	import Spinner from '$lib/components/Spinner.svelte';

	onMount(() => {
		getDiag();
	});

	onDestroy(() => {
	});

	let restPath = 'rest/spin-seq/diag';

	async function getDiag() {
		return getJsonRest(restPath, {}).then((data) => {
			console.log('Diag: ', data);
		});
	}

</script>

<SettingsCard>
	<span slot="title">Status</span>
	{#await getDiag()}
	<Spinner></Spinner>
	{:then diag}
	{/await}
</SettingsCard>