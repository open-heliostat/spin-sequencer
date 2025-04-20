<script lang="ts">
	import CanConsoleRestComp from '$lib/components/CANConsoleRestComp.svelte';
	import type { PageData } from '../$types';
	import Terminal from '~icons/tabler/terminal'

	export let data: PageData;
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<CanConsoleRestComp restPath={'/rest/spin-seq/can'} />
</div>
