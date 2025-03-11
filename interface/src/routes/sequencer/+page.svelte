<script lang="ts">
	import type { PageData } from '../$types';
	import SequencerRestComp from '$lib/components/SequencerRestComp.svelte';
	import Select from '$lib/components/Select.svelte';

	export let data: PageData;
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<SequencerRestComp restPath={'/rest/spin-seq/sequencer'} />
</div>
