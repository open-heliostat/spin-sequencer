<script lang="ts">
	import type { PageData } from '../$types';
	import SequencerRestComp from '$lib/components/SequencerRestComp.svelte';
	import MasterSequencerRestComp from '$lib/components/MasterSequencerRestComp.svelte';
	import Select from '$lib/components/Select.svelte';
	import ClassicControllerRestComp from '$lib/components/ClassicControllerRestComp.svelte';
	import IOSettingsRestComp from '$lib/components/IOSettingsRestComp.svelte';
	import { getJsonRest } from '$lib/stores/rest';

	export let data: PageData;

	async function getRemotesSettings() {
        return getJsonRest("rest/spin-seq/remotes/settings", {isMaster:false}).then((data) => {
			return data;
        });
    }
</script>

{#await getRemotesSettings() then data}
{#if data.isMaster}
	<MasterSequencerRestComp restPath={'/rest/spin-seq/sequencer/'} />
{/if}
{/await}

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<SequencerRestComp restPath={'/rest/spin-seq/sequencer/'} />
	<ClassicControllerRestComp restPath={'/rest/spin-seq/sequencer/controller/'} />
	<IOSettingsRestComp restPath={'/rest/spin-seq/io'} />
</div>
