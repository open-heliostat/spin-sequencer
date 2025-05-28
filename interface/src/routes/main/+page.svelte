<script lang="ts">
	import SequencerControl from '$lib/components/SequencerControl.svelte';
	import WelcomeTextDisplayComp from '$lib/components/WelcomeTextDisplayComp.svelte';
	import type { PageData } from '../$types';

	export let data: PageData;
</script>

<div
    class="fixed inset-0 z-50 flex flex-col space-y-4 bg-white dark:bg-stone-800 m-0 p-4"
>
	<WelcomeTextDisplayComp />
	<SequencerControl />
</div>
