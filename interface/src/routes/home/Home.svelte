<script lang="ts">
	import Restore from '$lib/components/RestoreComp.svelte';
	import StatusComp from '$lib/components/StatusComp.svelte';
	import WelcomeTextComp from '$lib/components/WelcomeTextComp.svelte';
</script>

<WelcomeTextComp />

<StatusComp restPath="rest/spin-seq/diag" />
<Restore></Restore>