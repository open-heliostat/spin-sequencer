<script lang="ts">
	import type { PageData } from '../$types';
	import Home from './Home.svelte';
	import Steppers from './Home.svelte';

	export let data: PageData;
</script>

<div
    class="fixed inset-0 z-50 flex flex-col space-y-4 bg-white m-0 p-4"
>
	<Home/>
</div>
