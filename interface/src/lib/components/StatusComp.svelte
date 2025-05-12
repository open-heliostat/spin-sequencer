<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { getJsonRest } from '$lib/stores/rest';
	import Spinner from '$lib/components/Spinner.svelte';
	import type { SpinDiagnostics } from '$lib/types/models';
	import Info from '~icons/tabler/info-circle';
	import StatusPanel from './StatusPanel.svelte';

	onMount(() => {
	});

	onDestroy(() => {
	});

	export let restPath = 'rest/spin-seq/diag';

	let diag: SpinDiagnostics = {} as SpinDiagnostics;

	async function getDiag() {
		return getJsonRest(restPath, diag).then((data) => {
			diag = data;
			return diag;
		});
	}

</script>

<SettingsCard>
	<Info slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Status</span>
	{#await getDiag()}
	<Spinner></Spinner>
	{:then diag}
		<StatusPanel
			diag={diag}
		/>
	{/await}
</SettingsCard>