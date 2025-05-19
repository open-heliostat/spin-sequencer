<script lang="ts">
	import type { PageData } from '../$types';
	import SequencerRestComp from '$lib/components/SequencerRestComp.svelte';
	import MasterSequencerRestComp from '$lib/components/MasterSequencerRestComp.svelte';
	import Select from '$lib/components/Select.svelte';
	import ClassicControllerRestComp from '$lib/components/ClassicControllerRestComp.svelte';

	export let data: PageData;
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<!-- <MasterSequencerRestComp restPath={'/rest/spin-seq/sequencer'} /> -->
	<SequencerRestComp restPath={'/rest/spin-seq/sequencer'} />
	<ClassicControllerRestComp restPath={'/rest/spin-seq/sequencer/controller'} />
</div>
